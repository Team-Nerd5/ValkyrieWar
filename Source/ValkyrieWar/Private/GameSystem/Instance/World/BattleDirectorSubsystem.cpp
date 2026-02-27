#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Unit/Component/UnitBrainComponent.h"
#include "Data/Struct/UnitEngagementSlotData.h"
#include "Interface/Unit/TargetReservationInterface.h"

static FORCEINLINE int32 TeamIndex(ETeam T) { return (T == ETeam::TeamA) ? 0 : 1; }

void UBattleDirectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bCleanupDirty = true;
    bCleanupInProgress = false;
    LastCleanupTime = -FLT_MAX;

    // 타이머는 Cleanup "실행"이 아니라 "요청 플래그"만 세팅
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().SetTimer(
            CleanupRequestTimerHandle,
            this,
            &UBattleDirectorSubsystem::RequestCleanupFromTimer,
            CleanupInterval,
            true
        );
    }
}

void UBattleDirectorSubsystem::Deinitialize()
{
    // 좀비 타이머 방지
    if (UWorld* W = GetWorld())
    {
        if (CleanupRequestTimerHandle.IsValid())
        {
            W->GetTimerManager().ClearTimer(CleanupRequestTimerHandle);
        }
    }

    TeamAUnits.Empty();
    TeamBUnits.Empty();
    TeamABase.Reset();
    TeamBBase.Reset();
    TeamAWallAnchors.Empty();
    TeamBWallAnchors.Empty();

    GridA.Empty();
    GridB.Empty();
    UnitToCell.Empty();

    Super::Deinitialize();
}

void UBattleDirectorSubsystem::TickCleanup()
{
    if (UWorld* W = GetWorld())
    {
        MaybeCleanup(W->GetTimeSeconds());
    }
}

void UBattleDirectorSubsystem::RequestCleanupFromTimer()
{
    // 타이머 콜백에서는 절대 컨테이너 변형(RemoveAll 등)을 하지 않는다.
    bCleanupDirty = true;
}

TArray<TWeakObjectPtr<AUnitCharacter>>& UBattleDirectorSubsystem::GetTeamArray(ETeam Team)
{
    return (Team == ETeam::TeamA) ? TeamAUnits : TeamBUnits;
}

const TArray<TWeakObjectPtr<AUnitCharacter>>& UBattleDirectorSubsystem::GetTeamArrayConst(ETeam Team) const
{
    return (Team == ETeam::TeamA) ? TeamAUnits : TeamBUnits;
}

TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>>& UBattleDirectorSubsystem::GetGrid(ETeam Team)
{
    return (Team == ETeam::TeamA) ? GridA : GridB;
}

const TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>>& UBattleDirectorSubsystem::GetGridConst(ETeam Team) const
{
    return (Team == ETeam::TeamA) ? GridA : GridB;
}

FIntPoint UBattleDirectorSubsystem::WorldToCell2D(const FVector& P) const
{
    const float Cell = FMath::Max(10.f, GridCellSize);
    const int32 X = FMath::FloorToInt(P.X / Cell);
    const int32 Y = FMath::FloorToInt(P.Y / Cell);
    return FIntPoint(X, Y);
}

void UBattleDirectorSubsystem::MarkCleanupDirty()
{
    bCleanupDirty = true;
}

void UBattleDirectorSubsystem::MaybeCleanup(float Now)
{
    if (!bCleanupDirty) return;
    if (bCleanupInProgress) return;
    if ((Now - LastCleanupTime) < CleanupInterval) return;

    bCleanupInProgress = true;
    CleanupInvalidReferences();
    bCleanupInProgress = false;

    LastCleanupTime = Now;
    bCleanupDirty = false;
}

void UBattleDirectorSubsystem::RegisterUnit(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    // AddUnique는 O(N)이라 우선 Add (중복등록이 없다는 전제)
    GetTeamArray(Unit->GetBrain()->Team).Add(Unit);

    // 적 베이스 링크
    Unit->GetBrain()->EnemyBase = GetEnemyBaseFor(Unit);

    // 그리드 등록
    UpdateUnitCell(Unit);

    MarkCleanupDirty();
}

void UBattleDirectorSubsystem::UnregisterUnit(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    // 내가 어떤 타깃에 예약돼 있었다면 해제 시도
    if (AUnitCharacter* Target = Cast<AUnitCharacter>(Unit->GetBrain()->ReservedTarget.Get()))
    {
        ReleaseReservation(Unit, Target);
    }

    // 그리드에서 제거
    RemoveUnitFromGrid(Unit);

    // 팀 배열에서 제거
    GetTeamArray(Unit->GetBrain()->Team).Remove(Unit);

    MarkCleanupDirty();
}

void UBattleDirectorSubsystem::RegisterWallAnchor(ETeam Team, AActor* AnchorActor)
{
    if (!AnchorActor) return;

    if (Team == ETeam::TeamA) TeamAWallAnchors.AddUnique(AnchorActor);
    else TeamBWallAnchors.AddUnique(AnchorActor);

    MarkCleanupDirty();
}

void UBattleDirectorSubsystem::UnregisterWallAnchor(ETeam Team, AActor* AnchorActor)
{
    if (!AnchorActor) return;

    if (Team == ETeam::TeamA) TeamAWallAnchors.Remove(AnchorActor);
    else TeamBWallAnchors.Remove(AnchorActor);

    MarkCleanupDirty();
}

const TArray<TWeakObjectPtr<AActor>>& UBattleDirectorSubsystem::GetTeamWallAnchorsConst(ETeam Team) const
{
    return (Team == ETeam::TeamA) ? TeamAWallAnchors : TeamBWallAnchors;
}

float UBattleDirectorSubsystem::DistanceToNearestWallAnchorSq(const FVector& P, ETeam WallTeam) const
{
    const TArray<TWeakObjectPtr<AActor>>& Anchors = GetTeamWallAnchorsConst(WallTeam);
    float BestSq = FLT_MAX;

    for (const TWeakObjectPtr<AActor>& W : Anchors)
    {
        const AActor* A = W.Get();
        if (!A) continue;

        const float D2 = FVector::DistSquared(P, A->GetActorLocation());
        if (D2 < BestSq) BestSq = D2;
    }
    return BestSq;
}

AActor* UBattleDirectorSubsystem::GetNearestWallAnchorTo(const FVector& From, ETeam Team) const
{
    const auto& Anchors = GetTeamWallAnchorsConst(Team);
    AActor* Best = nullptr;
    float BestSq = FLT_MAX;

    for (const auto& W : Anchors)
    {
        AActor* A = W.Get();
        if (!A) continue;

        const float D2 = FVector::DistSquared(From, A->GetActorLocation());
        if (D2 < BestSq)
        {
            BestSq = D2;
            Best = A;
        }
    }
    return Best;
}

AActor* UBattleDirectorSubsystem::GetEnemyBaseFor(const AUnitCharacter* Unit) const
{
    if (!Unit || !Unit->GetBrain()) return nullptr;

    const ETeam MyTeam = Unit->GetBrain()->Team;
    const ETeam EnemyTeam = (MyTeam == ETeam::TeamA) ? ETeam::TeamB : ETeam::TeamA;

    // 1) 적 팀의 WallAnchor 중 가장 가까운 앵커
    const auto& Anchors = GetTeamWallAnchorsConst(EnemyTeam);

    AActor* Best = nullptr;
    float BestSq = FLT_MAX;

    const FVector From = Unit->GetActorLocation();
    for (const auto& W : Anchors)
    {
        AActor* A = W.Get();
        if (!A) continue;

        const float D2 = FVector::DistSquared(From, A->GetActorLocation());
        if (D2 < BestSq)
        {
            BestSq = D2;
            Best = A;
        }
    }
    if (Best) return Best;

    return (EnemyTeam == ETeam::TeamA) ? TeamABase.Get() : TeamBBase.Get();
}

void UBattleDirectorSubsystem::CleanupInvalidReferences()
{
    auto CleanupTeam = [](TArray<TWeakObjectPtr<AUnitCharacter>>& Arr)
        {
            Arr.RemoveAll([](const TWeakObjectPtr<AUnitCharacter>& W)
                {
                    return !W.IsValid() || W->IsDead();
                });
        };

    auto CleanupAnchors = [](TArray<TWeakObjectPtr<AActor>>& Arr)
        {
            Arr.RemoveAll([](const TWeakObjectPtr<AActor>& W)
                {
                    return !W.IsValid();
                });
        };

    CleanupTeam(TeamAUnits);
    CleanupTeam(TeamBUnits);
    CleanupAnchors(TeamAWallAnchors);
    CleanupAnchors(TeamBWallAnchors);

    // 그리드도 무효 참조 제거 + 빈 셀 제거
    auto CleanupGrid = [](TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>>& Grid)
        {
            TArray<FIntPoint> ToRemove;
            for (auto& It : Grid)
            {
                It.Value.RemoveAll([](const TWeakObjectPtr<AUnitCharacter>& W)
                    {
                        return !W.IsValid() || W->IsDead();
                    });
                if (It.Value.Num() == 0) ToRemove.Add(It.Key);
            }
            for (const FIntPoint& K : ToRemove)
            {
                Grid.Remove(K);
            }
        };

    CleanupGrid(GridA);
    CleanupGrid(GridB);

    // UnitToCell도 정리
    TArray<TWeakObjectPtr<AUnitCharacter>> DeadKeys;
    DeadKeys.Reserve(UnitToCell.Num());

    for (auto& It : UnitToCell)
    {
        if (!It.Key.IsValid() || It.Key->IsDead())
        {
            DeadKeys.Add(It.Key);
        }
    }
    for (auto& K : DeadKeys)
    {
        UnitToCell.Remove(K);
    }
}

void UBattleDirectorSubsystem::NotifyTargetAssigned(AUnitCharacter* Unit, AActor* NewTarget)
{
    if (!Unit) return;

    if (Unit->GetClass()->ImplementsInterface(UTargetReservationInterface::StaticClass()))
    {
        ITargetReservationInterface::Execute_OnTargetAssigned(Unit, NewTarget);
    }
}

void UBattleDirectorSubsystem::DrawCellDebug(const FIntPoint& Cell, FColor Color, float Duration)
{
    if (!GetWorld()) return;

    const float CellSize = FMath::Max(10.f, GridCellSize);

    const FVector Min(Cell.X * CellSize, Cell.Y * CellSize, 0.f);
    const FVector Max((Cell.X + 1) * CellSize, (Cell.Y + 1) * CellSize, 0.f);

    const float Z = 50.f; // 바닥에서 약간 띄움

    const FVector A(Min.X, Min.Y, Z);
    const FVector B(Max.X, Min.Y, Z);
    const FVector C(Max.X, Max.Y, Z);
    const FVector D(Min.X, Max.Y, Z);

    DrawDebugLine(GetWorld(), A, B, Color, false, Duration, 0, 2.f);
    DrawDebugLine(GetWorld(), B, C, Color, false, Duration, 0, 2.f);
    DrawDebugLine(GetWorld(), C, D, Color, false, Duration, 0, 2.f);
    DrawDebugLine(GetWorld(), D, A, Color, false, Duration, 0, 2.f);
}

void UBattleDirectorSubsystem::DrawAllGridDebug()
{
    const float Duration = 0.f; // 0이면 한 프레임
    for (auto& Pair : GridA)
    {
        DrawCellDebug(Pair.Key, FColor::Red, Duration);
    }
    for (auto& Pair : GridB)
    {
        DrawCellDebug(Pair.Key, FColor::Blue, Duration);
    }
}

void UBattleDirectorSubsystem::RemoveUnitFromGrid(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    TWeakObjectPtr<AUnitCharacter> WUnit(Unit);

    FIntPoint* OldCell = UnitToCell.Find(WUnit);
    if (!OldCell) return;

    auto& Grid = GetGrid(Unit->GetBrain()->Team);

    if (TArray<TWeakObjectPtr<AUnitCharacter>>* List = Grid.Find(*OldCell))
    {
        // 중복/무효까지 제거
        List->RemoveAll([&](const TWeakObjectPtr<AUnitCharacter>& W)
            {
                return !W.IsValid() || W.Get() == Unit;
            });

        if (List->Num() == 0)
        {
            Grid.Remove(*OldCell);
        }
    }

    UnitToCell.Remove(WUnit);
}

void UBattleDirectorSubsystem::UpdateUnitCell(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    if (Unit->IsDead())
    {
        RemoveUnitFromGrid(Unit);
        return;
    }

    TWeakObjectPtr<AUnitCharacter> WUnit(Unit);
    const FIntPoint NewCell = WorldToCell2D(Unit->GetActorLocation());

    FIntPoint* OldCell = UnitToCell.Find(WUnit);
    if (OldCell && (*OldCell == NewCell))
        return; // 셀 변화 없음

    auto& Grid = GetGrid(Unit->GetBrain()->Team);

    // 이전 셀에서 제거
    if (OldCell)
    {
        if (TArray<TWeakObjectPtr<AUnitCharacter>>* List = Grid.Find(*OldCell))
        {
            List->RemoveAll([&](const TWeakObjectPtr<AUnitCharacter>& W)
                {
                    return !W.IsValid() || W.Get() == Unit;
                });

            if (List->Num() == 0)
            {
                Grid.Remove(*OldCell);
            }
        }
    }

    // 새 셀에 추가 (중복 방지)
    TArray<TWeakObjectPtr<AUnitCharacter>>& NewList = Grid.FindOrAdd(NewCell);
    NewList.AddUnique(WUnit);

    UnitToCell.Add(WUnit, NewCell);
}

void UBattleDirectorSubsystem::NotifyUnitMoved(AUnitCharacter* Unit)
{
    UpdateUnitCell(Unit);
}

void UBattleDirectorSubsystem::GatherEnemyCandidates(AUnitCharacter* Attacker, TArray<AUnitCharacter*>& OutCandidates) const
{
    OutCandidates.Reset();

    if (!Attacker || !Attacker->GetBrain()) return;

    const ETeam MyTeam = Attacker->GetBrain()->Team;
    const ETeam EnemyTeam = (MyTeam == ETeam::TeamA) ? ETeam::TeamB : ETeam::TeamA;

    const FIntPoint Center = WorldToCell2D(Attacker->GetActorLocation());

    const float Cell = FMath::Max(10.f, GridCellSize);
    const int32 NeedRadius = FMath::CeilToInt(SearchRadius / Cell);
    const int32 CellRadius = FMath::Clamp(NeedRadius, 1, FMath::Max(1, MaxCellRadius));

    const auto& EnemyGrid = GetGridConst(EnemyTeam);

    const int32 Cap = FMath::Max(8, CandidateCap);

    for (int32 dy = -CellRadius; dy <= CellRadius; ++dy)
    {
        for (int32 dx = -CellRadius; dx <= CellRadius; ++dx)
        {
            const FIntPoint Key(Center.X + dx, Center.Y + dy);

            const TArray<TWeakObjectPtr<AUnitCharacter>>* List = EnemyGrid.Find(Key);
            if (!List) continue;

            for (const auto& W : *List)
            {
                AUnitCharacter* E = W.Get();
                if (!E || E->IsDead()) continue;

                OutCandidates.Add(E);
                if (OutCandidates.Num() >= Cap)
                    return;
            }
        }
    }
}

AUnitCharacter* UBattleDirectorSubsystem::FindBestTargetWithFreeSlot_Grid(
    AUnitCharacter* Attacker,
    float Now,
    const TArray<AUnitCharacter*>& Candidates
) const
{
    if (!Attacker || !Attacker->GetBrain()) return nullptr;

    const FVector MyLoc = Attacker->GetActorLocation();
    const float SearchRadiusSq = FMath::Max(1.f, SearchRadius * SearchRadius);
    const float AttackRangeSq = Attacker->GetAttackRange() * Attacker->GetAttackRange();
    const float NearWallDistSq = NearWallDistance * NearWallDistance;

    const ETeam MyTeam = Attacker->GetBrain()->Team;
    const ETargetingPolicy Policy = Attacker->GetBrain()->TargetingPolicy;

    // 0~1 기반(스케일 독립) 가중치
    constexpr float W_Distance = 1.0f;
    constexpr float W_FreeSlots = 0.3f;
    constexpr float W_NearWall = 1.5f;

    AActor* MyNearestWall = GetNearestWallAnchorTo(MyLoc, MyTeam);

    AUnitCharacter* Best = nullptr;
    float BestScore = -FLT_MAX;

    for (AUnitCharacter* E : Candidates)
    {
        if (!E || E->IsDead()) continue;
        if (!E->HasFreeSlot()) continue;

        const float DistSq = FVector::DistSquared(MyLoc, E->GetActorLocation());
        if (DistSq > SearchRadiusSq) continue;

        // 정책별 제약
        if (Policy == ETargetingPolicy::FarthestInAttackRangeThenBase)
        {
            if (DistSq > AttackRangeSq) continue;
        }

        // 가까울수록 1, 멀수록 0
        const float DistanceScore = 1.0f - (DistSq / SearchRadiusSq);

        // 빈 슬롯 비율(0~1)
        float FreeSlotsScore = 0.f;
        {
            int32 FreeCount = 0;
            for (const FUnitEngagementSlotData& Slot : E->EngagementSlots)
            {
                if (!Slot.Attacker.IsValid()) ++FreeCount;
            }
            FreeSlotsScore = (E->EngagementSlots.Num() > 0)
                ? (float)FreeCount / (float)E->EngagementSlots.Num()
                : 0.f;
        }

        // 성벽 근처(0/1)
        float NearWallScore = 0.f;
        if (Policy == ETargetingPolicy::NearWallEnemyThenBase || Policy == ETargetingPolicy::Auto)
        {
            float ToWallSq = FLT_MAX;
            if (MyNearestWall)
            {
                ToWallSq = FVector::DistSquared(E->GetActorLocation(), MyNearestWall->GetActorLocation());
            }
            else
            {
                ToWallSq = DistanceToNearestWallAnchorSq(E->GetActorLocation(), MyTeam);
            }

            if (ToWallSq <= NearWallDistSq)
            {
                NearWallScore = 1.f;
            }
        }

        float Score = 0.f;

        if (Policy == ETargetingPolicy::FarthestInAttackRangeThenBase)
        {
            // 멀수록 우선(0~1)
            const float FarScore = FMath::Clamp(DistSq / FMath::Max(1.f, AttackRangeSq), 0.f, 1.f);
            Score = FarScore * W_Distance + FreeSlotsScore * W_FreeSlots;
        }
        else
        {
            Score = DistanceScore * W_Distance
                + FreeSlotsScore * W_FreeSlots
                + NearWallScore * W_NearWall;
        }

        if (Score > BestScore)
        {
            BestScore = Score;
            Best = E;
        }
    }

    return Best;
}

bool UBattleDirectorSubsystem::TryReserve(AUnitCharacter* Attacker, AUnitCharacter* Target, float Now)
{
    if (!Attacker || !Target || !Attacker->GetBrain()) return false;
    if (Attacker->IsDead() || Target->IsDead()) return false;

    // 이미 같은 타겟이어도 슬롯에 진짜 들어있는지 확인
    if (Attacker->GetBrain()->ReservedTarget.Get() == Target)
    {
        if (Target->FindSlotOfAttacker(Attacker) != INDEX_NONE)
            return true;
        // 슬롯이 없으면 아래 로직으로 재등록 시도
    }

    if (!Attacker->GetBrain()->CanChangeReservation(Now))
        return false;

    const int32 SlotIdx = Target->FindFirstFreeSlot();
    if (SlotIdx == INDEX_NONE) return false;

    if (AUnitCharacter* OldTarget = Cast<AUnitCharacter>(Attacker->GetBrain()->ReservedTarget.Get()))
    {
        ReleaseReservation(Attacker, OldTarget);
    }

    Target->EngagementSlots[SlotIdx].Attacker = Attacker;
    Target->EngagementSlots[SlotIdx].ReservedAtTime = Now;

    Attacker->GetBrain()->SetReservedTarget(Target, Now);
    NotifyTargetAssigned(Attacker, Target);

    return true;
}

void UBattleDirectorSubsystem::ReleaseReservation(AUnitCharacter* Attacker, AUnitCharacter* Target)
{
    if (!Attacker || !Target || !Attacker->GetBrain()) return;

    const int32 SlotIdx = Target->FindSlotOfAttacker(Attacker);
    if (SlotIdx != INDEX_NONE)
    {
        Target->EngagementSlots[SlotIdx].Reset();
    }

    if (Attacker->GetBrain()->ReservedTarget.Get() == Target)
    {
        const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
        Attacker->GetBrain()->ClearReservedTarget(Now);
        NotifyTargetAssigned(Attacker, nullptr);
    }
}

void UBattleDirectorSubsystem::ReleaseAllAttackersOfTarget(AUnitCharacter* Target)
{
    if (!Target) return;

    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

    for (FUnitEngagementSlotData& Slot : Target->EngagementSlots)
    {
        if (AActor* AttackerActor = Slot.Attacker.Get())
        {
            if (AUnitCharacter* Attacker = Cast<AUnitCharacter>(AttackerActor))
            {
                if (Attacker->GetBrain() && Attacker->GetBrain()->ReservedTarget.Get() == Target)
                {
                    Attacker->GetBrain()->ClearReservedTarget(Now);
                    NotifyTargetAssigned(Attacker, nullptr);
                }
            }
        }
        Slot.Reset();
    }
}

void UBattleDirectorSubsystem::UpdateReservationFor(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

    // 여기서 MaybeCleanup() 호출하지 않음
    // 중앙 루프에서 BD->TickCleanup() 1회 호출로만 실행

    // 이 유닛만 그리드 셀 업데이트
    UpdateUnitCell(Unit);

    // 현재 예약 타깃이 유효하면 유지
    if (AUnitCharacter* CurrentTarget = Cast<AUnitCharacter>(Unit->GetBrain()->ReservedTarget.Get()))
    {
        if (!CurrentTarget->IsDead())
        {
            if (CurrentTarget->FindSlotOfAttacker(Unit) != INDEX_NONE)
                return;

            Unit->GetBrain()->ClearReservedTarget(Now);
            NotifyTargetAssigned(Unit, nullptr);
        }
        else
        {
            Unit->GetBrain()->ClearReservedTarget(Now);
            NotifyTargetAssigned(Unit, nullptr);
        }
    }

    // 후보를 그리드 인접 셀에서만 수집
    TArray<AUnitCharacter*> Candidates;
    GatherEnemyCandidates(Unit, Candidates);

    if (Candidates.Num() > 0)
    {
        if (AUnitCharacter* Best = FindBestTargetWithFreeSlot_Grid(Unit, Now, Candidates))
        {
            TryReserve(Unit, Best, Now);
            return;
        }
    }

    // 후보가 없거나 모두 슬롯 없음 → 베이스 전진
    Unit->GetBrain()->ReservationState = EReservationState::None;
    Unit->GetBrain()->EnemyBase = GetEnemyBaseFor(Unit);
}
