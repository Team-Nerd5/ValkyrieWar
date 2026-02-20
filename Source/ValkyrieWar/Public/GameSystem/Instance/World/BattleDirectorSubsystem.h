// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/Enum/CommonEnums.h"
#include "BattleDirectorSubsystem.generated.h"

class AUnitCharacter;

/**
 * BattleDirectorSubsystem
 *
 * “전투 주선자” 역할을 수행하는 중앙 매칭 시스템.
 * 유닛 간 1:1 ~ 1:N 교전을 슬롯 기반으로 할당하며,
 * 전장 밀집도와 성벽 근접도를 고려한 정책 기반 타깃 선정을 수행한다.
 * 이를 통해 과도한 다대일 타겟팅을 원천 차단하고 최대한 자연스러운 난전 라인을 형성.
 *
 * 개별 유닛 AI는 판단을 위임하고,
 * 본 서브시스템이 전투 관계의 일관성과 안정성을 보장한다.
 */
UCLASS()
class VALKYRIEWAR_API UBattleDirectorSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void RegisterUnit(AUnitCharacter* Unit);
    void UnregisterUnit(AUnitCharacter* Unit);

    // 성벽(또는 성벽 라인) 앵커 등록
    void RegisterWallAnchor(ETeam Team, AActor* AnchorActor);
    void UnregisterWallAnchor(ETeam Team, AActor* AnchorActor);

    // BT 서비스에서 호출: 해당 유닛의 예약을 업데이트
    void UpdateReservationFor(AUnitCharacter* Unit);

    // 정리: 타깃이 죽거나 EndPlay일 때 호출
    void ReleaseAllAttackersOfTarget(AUnitCharacter* Target);
    void ReleaseReservation(AUnitCharacter* Attacker, AUnitCharacter* Target);

private:
    TArray<TWeakObjectPtr<AUnitCharacter>>& GetTeamArray(ETeam Team);
    const TArray<TWeakObjectPtr<AUnitCharacter>>& GetTeamArrayConst(ETeam Team) const;

    // '적 기지' 개념
    // 기본은 '적 성벽(Enemy Team WallAnchor 중 가장 가까운 앵커)'을 목표로 삼는다.
    // 성벽 앵커가 없으면(테스트/예외) 등록된 TeamBase(옵션)로 폴백.
    AActor* GetEnemyBaseFor(const AUnitCharacter* Unit) const;

    const TArray<TWeakObjectPtr<AActor>>& GetTeamWallAnchorsConst(ETeam Team) const;
    float DistanceToNearestWallAnchorSq(const FVector& P, ETeam WallTeam) const;

    // 후보 선택 (TargetingPolicy 반영)
    AUnitCharacter* FindBestTargetWithFreeSlot(AUnitCharacter* Attacker, float Now) const;

    // 예약 실행(원자적으로)
    bool TryReserve(AUnitCharacter* Attacker, AUnitCharacter* Target, float Now);
    void CleanupInvalidReferences();

    void NotifyTargetAssigned(AUnitCharacter* Unit, AActor* NewTarget);

public:
    // 매칭 갱신 주기
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float UpdateInterval = 0.25f;

    // 탐색 반경(실질적으로는 상대 타겟팅 허용 범위)
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float SearchRadius = 2000.f;

    // "성벽에서 가까운 적" 판단 거리(내 WallAnchor들 중 가장 가까운 지점까지의 거리)
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NearWallDistance = 900.f;

    //“빈 슬롯이 없으면” 대기(또는 베이스 전진)로 전환하기 전 유예
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NoSlotGraceSeconds = 0.6f;

private:
    // 팀별 유닛 목록
    UPROPERTY()
    TArray<TWeakObjectPtr<AUnitCharacter>> TeamAUnits;

    UPROPERTY()
    TArray<TWeakObjectPtr<AUnitCharacter>> TeamBUnits;

    UPROPERTY()
    TWeakObjectPtr<AActor> TeamABase;

    UPROPERTY()
    TWeakObjectPtr<AActor> TeamBBase;

    // 팀별 성벽 앵커(Weak로 안전)
    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> TeamAWallAnchors;

    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> TeamBWallAnchors;
};
