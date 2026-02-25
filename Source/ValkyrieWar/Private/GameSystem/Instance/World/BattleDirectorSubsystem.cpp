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
    LastCleanupTime = -FLT_MAX;
}

void UBattleDirectorSubsystem::Deinitialize()
{
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
    if ((Now - LastCleanupTime) < CleanupInterval) return;

    CleanupInvalidReferences();
    LastCleanupTime = Now;
    bCleanupDirty = false;
}

void UBattleDirectorSubsystem::RegisterUnit(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    // AddUnique는 O(N)이라 우선 Add로 (중복등록이 없다는 전제)
    GetTeamArray(Unit->GetBrain()->Team).Add(Unit);

    // 적 베이스 링크
    Unit->GetBrain()->EnemyBase = GetEnemyBaseFor(Unit);

    // 그리드 등록
    UpdateUnitCell(Unit);

    MarkCleanupDirty();
}

void UBattleDirectorSubsystem::RemoveUnitFromGrid(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    TWeakObjectPtr<AUnitCharacter> WUnit(Unit);

    FIntPoint* OldCell = UnitToCell.Find(WUnit);
    if (!OldCell)
        return;

    auto& Grid = GetGrid(Unit->GetBrain()->Team);
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

    UnitToCell.Remove(WUnit);
}

void UBattleDirectorSubsystem::UpdateUnitCell(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain() || Unit->IsDead()) return;

    TWeakObjectPtr<AUnitCharacter> WUnit(Unit);
    const FIntPoint NewCell = WorldToCell2D(Unit->GetActorLocation());

    FIntPoint* OldCell = UnitToCell.Find(WUnit);
    if (OldCell && (*OldCell == NewCell))
        return; // 셀 변화 없음

    // 이전 셀에서 제거
    if (OldCell)
    {
        auto& Grid = GetGrid(Unit->GetBrain()->Team);
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

    // 새 셀에 추가
    auto& NewList = GetGrid(Unit->GetBrain()->Team).FindOrAdd(NewCell);
    NewList.Add(WUnit);
    UnitToCell.Add(WUnit, NewCell);
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

void UBattleDirectorSubsystem::GatherEnemyCandidates(AUnitCharacter* Attacker, TArray<AUnitCharacter*>& OutCandidates) const
{
    OutCandidates.Reset();

    if (!Attacker || !Attacker->GetBrain()) return;

    const ETeam MyTeam = Attacker->GetBrain()->Team;
    const ETeam EnemyTeam = (MyTeam == ETeam::TeamA) ? ETeam::TeamB : ETeam::TeamA;

    // Attacker 셀 (역인덱스 없으면 직접 계산)
    const FIntPoint Center = WorldToCell2D(Attacker->GetActorLocation());

    const float Cell = FMath::Max(10.f, GridCellSize);
    const int32 NeedRadius = FMath::CeilToInt(SearchRadius / Cell);
    const int32 CellRadius = FMath::Clamp(NeedRadius, 1, FMath::Max(1, MaxCellRadius));

    const auto& EnemyGrid = GetGridConst(EnemyTeam);

    // 후보
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
                    return; // cap 도달 시 조기 종료
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

    const float SearchRadiusSq = SearchRadius * SearchRadius;
    const float AttackRangeSq = Attacker->GetAttackRange() * Attacker->GetAttackRange();
    const float NearWallDistSq = NearWallDistance * NearWallDistance;

    const ETeam MyTeam = Attacker->GetBrain()->Team;
    const ETargetingPolicy Policy = Attacker->GetBrain()->TargetingPolicy;

    // 후보마다 앵커 전체를 돌지 않도록: Attacker 기준 가장 가까운 내 앵커 1개만
    AActor* MyNearestWall = GetNearestWallAnchorTo(MyLoc, MyTeam);

    AUnitCharacter* Best = nullptr;
    float BestScore = -FLT_MAX;

    for (AUnitCharacter* E : Candidates)
    {
        if (!E || E->IsDead()) continue;
        if (!E->HasFreeSlot()) continue;

        const float DistSq = FVector::DistSquared(MyLoc, E->GetActorLocation());
        if (DistSq > SearchRadiusSq) continue;

        // 슬롯 보너스(상수)
        const int32 FreeIdx = E->FindFirstFreeSlot();
        const float FreeBonus = (FreeIdx != INDEX_NONE) ? 50.f : 0.f;

        // 성벽 근처 판정(빠른 버전)
        bool bNearMyWall = false;
        float ToWallSq = FLT_MAX;
        if (MyNearestWall)
        {
            ToWallSq = FVector::DistSquared(E->GetActorLocation(), MyNearestWall->GetActorLocation());
            bNearMyWall = (ToWallSq <= NearWallDistSq);
        }

        // sqrt 제거: -DistSq 형태로 비교
        float Score = -DistSq + FreeBonus;

        switch (Policy)
        {
        case ETargetingPolicy::NearWallEnemyThenBase:
        {
            if (bNearMyWall)
            {
                Score += 100000.f;
                Score += -ToWallSq * 0.01f; // sqrt 대신 ToWallSq 사용(가중치만 조절)
            }
            break;
        }
        case ETargetingPolicy::NearestEnemyThenBase:
        {
            // 기본 Score 사용
            break;
        }
        case ETargetingPolicy::FarthestInAttackRangeThenBase:
        {
            if (DistSq > AttackRangeSq) continue;
            Score = DistSq + FreeBonus; // 멀수록 우선 (sqrt 제거)
            break;
        }
        case ETargetingPolicy::Auto:
        default:
        {
            // 안전 폴백: NearWall 우선
            if (bNearMyWall)
            {
                Score += 100000.f;
                Score += -ToWallSq * 0.01f;
            }
            break;
        }
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

    if (Attacker->GetBrain()->ReservedTarget.Get() == Target)
        return true;

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

void UBattleDirectorSubsystem::NotifyUnitMoved(AUnitCharacter* Unit)
{
    if (!Unit || !Unit->GetBrain()) return;

    // 죽었으면 그리드에서 제거
    if (Unit->IsDead())
    {
        RemoveUnitFromGrid(Unit);
        return;
    }

    const ETeam Team = Unit->GetBrain()->Team;
    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>>& Grid = GetGrid(Team);

    const FIntPoint NewCell = WorldToCell2D(Unit->GetActorLocation());
    TWeakObjectPtr<AUnitCharacter> WUnit(Unit);

    FIntPoint* OldCell = UnitToCell.Find(WUnit);

    // 셀 변동 없음 → 아무 것도 할 일 없음
    if (OldCell && (*OldCell == NewCell))
        return;

    // 이전 셀에서 제거
    if (OldCell)
    {
        if (TArray<TWeakObjectPtr<AUnitCharacter>>* OldList = Grid.Find(*OldCell))
        {
            OldList->RemoveAll([&](const TWeakObjectPtr<AUnitCharacter>& W)
                {
                    return !W.IsValid() || W.Get() == Unit;
                });

            if (OldList->Num() == 0)
                Grid.Remove(*OldCell);
        }
    }

    // 새 셀에 추가
    TArray<TWeakObjectPtr<AUnitCharacter>>& NewList = Grid.FindOrAdd(NewCell);
    NewList.Add(WUnit);

    UnitToCell.Add(WUnit, NewCell);
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

    // 유닛별 전체 정리 제거 → 주기 정리
    MaybeCleanup(Now);

    // 호출될 때마다 “이 유닛만” 그리드 셀 업데이트
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

    // ★ 후보를 “그리드 인접 셀”에서만 수집
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
