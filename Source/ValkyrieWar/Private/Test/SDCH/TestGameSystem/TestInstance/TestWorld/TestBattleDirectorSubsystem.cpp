// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestGameSystem/TestInstance/TestWorld/TestBattleDirectorSubsystem.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "Test/SDCH/TestUnit/TestComponents/TestUnitBrainComponent.h"
#include "Test/SDCH/TestInterface/TestTargetReservationInterface.h"


void UTestBattleDirectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UTestBattleDirectorSubsystem::Deinitialize()
{
    TeamAUnits.Empty();
    TeamBUnits.Empty();
    TeamABase.Reset();
    TeamBBase.Reset();
    TeamAWallAnchors.Empty();
    TeamBWallAnchors.Empty();
    Super::Deinitialize();
}

TArray<TWeakObjectPtr<ATestBaseUnit>>& UTestBattleDirectorSubsystem::GetTeamArray(ETeam Team)
{
    return (Team == ETeam::TeamA) ? TeamAUnits : TeamBUnits;
}

const TArray<TWeakObjectPtr<ATestBaseUnit>>& UTestBattleDirectorSubsystem::GetTeamArrayConst(ETeam Team) const
{
    return (Team == ETeam::TeamA) ? TeamAUnits : TeamBUnits;
}

void UTestBattleDirectorSubsystem::RegisterUnit(ATestBaseUnit* Unit)
{
    if (!Unit || !Unit->Brain) return;
    GetTeamArray(Unit->Brain->Team).AddUnique(Unit);

    // 적 베이스 링크
    Unit->Brain->EnemyBase = GetEnemyBaseFor(Unit);
}

void UTestBattleDirectorSubsystem::UnregisterUnit(ATestBaseUnit* Unit)
{
    if (!Unit || !Unit->Brain) return;

    // 내가 어떤 타깃에 예약돼 있었다면 해제 시도
    if (ATestBaseUnit* Target = Cast<ATestBaseUnit>(Unit->Brain->ReservedTarget.Get()))
    {
        ReleaseReservation(Unit, Target);
    }

    GetTeamArray(Unit->Brain->Team).Remove(Unit);
}

void UTestBattleDirectorSubsystem::RegisterTeamBase(ETeam Team, AActor* BaseActor)
{
    if (Team == ETeam::TeamA) TeamABase = BaseActor;
    else TeamBBase = BaseActor;

    // 이미 등록된 유닛들의 EnemyBase도 갱신
    for (TWeakObjectPtr<ATestBaseUnit> W : TeamAUnits)
    {
        if (ATestBaseUnit* U = W.Get())
            if (U->Brain) U->Brain->EnemyBase = GetEnemyBaseFor(U);
    }
    for (TWeakObjectPtr<ATestBaseUnit> W : TeamBUnits)
    {
        if (ATestBaseUnit* U = W.Get())
            if (U->Brain) U->Brain->EnemyBase = GetEnemyBaseFor(U);
    }
}

void UTestBattleDirectorSubsystem::RegisterWallAnchor(ETeam Team, AActor* AnchorActor)
{
    if (!AnchorActor) return;
    if (Team == ETeam::TeamA) TeamAWallAnchors.AddUnique(AnchorActor);
    else TeamBWallAnchors.AddUnique(AnchorActor);
}

void UTestBattleDirectorSubsystem::UnregisterWallAnchor(ETeam Team, AActor* AnchorActor)
{
    if (!AnchorActor) return;
    if (Team == ETeam::TeamA) TeamAWallAnchors.Remove(AnchorActor);
    else TeamBWallAnchors.Remove(AnchorActor);
}

const TArray<TWeakObjectPtr<AActor>>& UTestBattleDirectorSubsystem::GetTeamWallAnchorsConst(ETeam Team) const
{
    return (Team == ETeam::TeamA) ? TeamAWallAnchors : TeamBWallAnchors;
}

float UTestBattleDirectorSubsystem::DistanceToNearestWallAnchorSq(const FVector& P, ETeam WallTeam) const
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

AActor* UTestBattleDirectorSubsystem::GetEnemyBaseFor(const ATestBaseUnit* Unit) const
{
    if (!Unit || !Unit->Brain) return nullptr;

    const ETeam MyTeam = Unit->Brain->Team;
    const ETeam EnemyTeam = (MyTeam == ETeam::TeamA) ? ETeam::TeamB : ETeam::TeamA;

    // 1) 성벽을 적 기지로 취급: 적 팀의 WallAnchor 중 가장 가까운 앵커를 반환
    const TArray<TWeakObjectPtr<AActor>>& Anchors = GetTeamWallAnchorsConst(EnemyTeam);
    AActor* Best = nullptr;
    float BestSq = FLT_MAX;

    const FVector From = Unit->GetActorLocation();
    for (const TWeakObjectPtr<AActor>& W : Anchors)
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

    // 2) 폴백: 등록된 TeamBase(옵션). (테스트/임시용)
    return (EnemyTeam == ETeam::TeamA) ? TeamABase.Get() : TeamBBase.Get();
}

void UTestBattleDirectorSubsystem::CleanupInvalidReferences()
{
    auto CleanupTeam = [](TArray<TWeakObjectPtr<ATestBaseUnit>>& Arr)
        {
            Arr.RemoveAll([](const TWeakObjectPtr<ATestBaseUnit>& W)
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
}

void UTestBattleDirectorSubsystem::NotifyTargetAssigned(ATestBaseUnit* Unit, AActor* NewTarget)
{
    if (!Unit) return;

    if (Unit->GetClass()->ImplementsInterface(UTestTargetReservationInterface::StaticClass()))
    {
        ITestTargetReservationInterface::Execute_OnTargetAssigned(Unit, NewTarget);
    }
}

ATestBaseUnit* UTestBattleDirectorSubsystem::FindBestTargetWithFreeSlot(ATestBaseUnit* Attacker, float Now) const
{
    if (!Attacker || !Attacker->Brain) return nullptr;

    const ETeam MyTeam = Attacker->Brain->Team;
    const ETeam EnemyTeam = (MyTeam == ETeam::TeamA) ? ETeam::TeamB : ETeam::TeamA;

    const TArray<TWeakObjectPtr<ATestBaseUnit>>& Enemies = GetTeamArrayConst(EnemyTeam);

    const FVector MyLoc = Attacker->GetActorLocation();

    // NOTE:
    // - 서브시스템은 "슬롯/attacker 제한" 정책을 유지한다.
    // - 여기서는 "어떤 후보를 우선할지"만 (TargetingPolicy)로 결정한다.
    const ETargetingPolicy Policy = Attacker->Brain->TargetingPolicy;

    const float NearWallDistSq = NearWallDistance * NearWallDistance;
    const float AttackRangeSq = Attacker->AttackRange * Attacker->AttackRange;

    ATestBaseUnit* Best = nullptr;
    float BestScore = -FLT_MAX;

    for (const TWeakObjectPtr<ATestBaseUnit>& WEnemy : Enemies)
    {
        ATestBaseUnit* E = WEnemy.Get();
        if (!E || E->IsDead()) continue;

        // 슬롯이 없으면 제외
        if (!E->HasFreeSlot()) continue;

        const float DistSq = FVector::DistSquared(MyLoc, E->GetActorLocation());
        if (DistSq > (SearchRadius * SearchRadius)) continue;

        // --- 성벽 근처 판정(내 팀의 WallAnchor들 기준) ---
        // WallAnchor가 하나도 없으면 이 조건은 자동으로 false (BestSq = FLT_MAX)
        const float ToWallSq = DistanceToNearestWallAnchorSq(E->GetActorLocation(), MyTeam);
        const bool bNearMyWall = (ToWallSq <= NearWallDistSq);

        // --- 기본 점수 요소(슬롯 여유) ---
        const int32 FreeIdx = E->FindFirstFreeSlot();
        const float FreeBonus = (FreeIdx != INDEX_NONE) ? 50.f : 0.f;

        // --- 정책별 스코어링 ---
        float Score = -FMath::Sqrt(DistSq) + FreeBonus; // 기본: 가까울수록 좋다

        switch (Policy)
        {
        case ETargetingPolicy::NearWallEnemyThenBase:
        {
            // "성벽에서 가까운 적"을 강하게 우선
            // 1) 성벽 근처면 큰 보너스
            // 2) 같은 성벽 근처 그룹에서는 "성벽에 더 가까운 적"을 더 우선
            if (bNearMyWall)
            {
                Score += 100000.f;
                // 성벽에 더 가까울수록 점수가 커지도록 (-sqrt(ToWallSq))
                Score += -FMath::Sqrt(ToWallSq) * 10.f;
            }
            break;
        }
        case ETargetingPolicy::NearestEnemyThenBase:
        {
            // 기본 Score 그대로 사용
            break;
        }
        case ETargetingPolicy::FarthestInAttackRangeThenBase:
        {
            // 공격 사거리 안에서만 고려하고, 멀수록 우선
            if (DistSq > AttackRangeSq)
            {
                continue; // 사거리 밖은 후보 제외
            }
            // 멀수록 점수 증가
            Score = FMath::Sqrt(DistSq) + FreeBonus;
            break;
        }
        case ETargetingPolicy::Auto:
        default:
        {
            // Auto는 BeginPlay에서 NearWallEnemyThenBase로 내려주는 것이 기본이지만,
            // 혹시 남아있으면 안전하게 NearWallEnemyThenBase 취급
            if (bNearMyWall)
            {
                Score += 100000.f;
                Score += -FMath::Sqrt(ToWallSq) * 10.f;
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

bool UTestBattleDirectorSubsystem::TryReserve(ATestBaseUnit* Attacker, ATestBaseUnit* Target, float Now)
{
    if (!Attacker || !Target || !Attacker->Brain) return false;
    if (Attacker->IsDead() || Target->IsDead()) return false;

    // 이미 같은 타깃 예약 중이면 OK
    if (Attacker->Brain->ReservedTarget.Get() == Target)
        return true;

    // thrashing 방지
    if (!Attacker->Brain->CanChangeReservation(Now))
        return false;

    // 타깃의 슬롯 확보
    const int32 SlotIdx = Target->FindFirstFreeSlot();
    if (SlotIdx == INDEX_NONE) return false;

    // 기존 예약이 있으면 해제
    if (ATestBaseUnit* OldTarget = Cast<ATestBaseUnit>(Attacker->Brain->ReservedTarget.Get()))
    {
        ReleaseReservation(Attacker, OldTarget);
    }

    // 원자적으로 등록
    Target->EngagementSlots[SlotIdx].Attacker = Attacker;
    Target->EngagementSlots[SlotIdx].ReservedAtTime = Now;

    Attacker->Brain->SetReservedTarget(Target, Now);

    NotifyTargetAssigned(Attacker, Target);

    return true;
}

void UTestBattleDirectorSubsystem::ReleaseReservation(ATestBaseUnit* Attacker, ATestBaseUnit* Target)
{
    if (!Attacker || !Target || !Attacker->Brain) return;

    const int32 SlotIdx = Target->FindSlotOfAttacker(Attacker);
    if (SlotIdx != INDEX_NONE)
    {
        Target->EngagementSlots[SlotIdx].Reset();
    }

    // 공격자 쪽도 비움
    if (Attacker->Brain->ReservedTarget.Get() == Target)
    {
        const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
        Attacker->Brain->ClearReservedTarget(Now);

        NotifyTargetAssigned(Attacker, nullptr);
    }
}

void UTestBattleDirectorSubsystem::ReleaseAllAttackersOfTarget(ATestBaseUnit* Target)
{
    if (!Target) return;

    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

    for (FTestEngagementSlot& Slot : Target->EngagementSlots)
    {
        if (AActor* AttackerActor = Slot.Attacker.Get())
        {
            if (ATestBaseUnit* Attacker = Cast<ATestBaseUnit>(AttackerActor))
            {
                if (Attacker->Brain && Attacker->Brain->ReservedTarget.Get() == Target)
                {
                    Attacker->Brain->ClearReservedTarget(Now);

                    NotifyTargetAssigned(Attacker, nullptr);
                }
            }
        }
        Slot.Reset();
    }
}

void UTestBattleDirectorSubsystem::UpdateReservationFor(ATestBaseUnit* Unit)
{
    if (!Unit || !Unit->Brain) return;

    CleanupInvalidReferences();

    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

    // 현재 예약 타깃이 유효하면 유지
    if (ATestBaseUnit* CurrentTarget = Cast<ATestBaseUnit>(Unit->Brain->ReservedTarget.Get()))
    {
        if (!CurrentTarget->IsDead())
        {
            // 내가 타깃 슬롯에 실제로 등록돼 있는지 확인(꼬임 방지)
            if (CurrentTarget->FindSlotOfAttacker(Unit) != INDEX_NONE)
                return;

            // 슬롯에서 빠져있으면 예약이 깨진 것 -> 비우고 새로 잡자
            Unit->Brain->ClearReservedTarget(Now);
            NotifyTargetAssigned(Unit, nullptr);
        }
        else
        {
            Unit->Brain->ClearReservedTarget(Now);
            NotifyTargetAssigned(Unit, nullptr);
        }
    }

    // 새 타깃 찾기
    if (ATestBaseUnit* Best = FindBestTargetWithFreeSlot(Unit, Now))
    {
        TryReserve(Unit, Best, Now);
    }
    else
    {
        // 아무도 빈 슬롯 없으면: 예약 없음 (BT가 “베이스로 전진” 선택)
        // 필요하면 여기서 “짧은 대기” 상태를 따로 두면 더 자연스러움
        Unit->Brain->ReservationState = EReservationState::None;
        Unit->Brain->EnemyBase = GetEnemyBaseFor(Unit);
    }
}
