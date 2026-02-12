// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/Enum/CommonEnums.h"
#include "TestBattleDirectorSubsystem.generated.h"

class ATestBaseUnit;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTestBattleDirectorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
    // 매칭 갱신 주기
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float UpdateInterval = 0.25f;

    // 탐색 반경
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float SearchRadius = 2000.f;

    // "성벽에서 가까운 적" 판단 거리(내 WallAnchor들 중 가장 가까운 지점까지의 거리)
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NearWallDistance = 900.f;

    //“빈 슬롯이 없으면” 대기(또는 베이스 전진)로 전환하기 전 유예
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NoSlotGraceSeconds = 0.6f;

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void RegisterUnit(ATestBaseUnit* Unit);
    void UnregisterUnit(ATestBaseUnit* Unit);

    // 성벽(또는 성벽 라인) 앵커 등록
    void RegisterWallAnchor(ETeam Team, AActor* AnchorActor);
    void UnregisterWallAnchor(ETeam Team, AActor* AnchorActor);

    // BT 서비스에서 호출: 해당 유닛의 예약을 업데이트
    void UpdateReservationFor(ATestBaseUnit* Unit);

    // 정리: 타깃이 죽거나 EndPlay일 때 호출
    void ReleaseAllAttackersOfTarget(ATestBaseUnit* Target);
    void ReleaseReservation(ATestBaseUnit* Attacker, ATestBaseUnit* Target);

private:
    // 팀별 유닛 목록
    UPROPERTY()
    TArray<TWeakObjectPtr<ATestBaseUnit>> TeamAUnits;

    UPROPERTY()
    TArray<TWeakObjectPtr<ATestBaseUnit>> TeamBUnits;

    UPROPERTY()
    TWeakObjectPtr<AActor> TeamABase;

    UPROPERTY()
    TWeakObjectPtr<AActor> TeamBBase;

    // 팀별 성벽 앵커(Weak로 안전)
    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> TeamAWallAnchors;

    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> TeamBWallAnchors;

private:
    TArray<TWeakObjectPtr<ATestBaseUnit>>& GetTeamArray(ETeam Team);
    const TArray<TWeakObjectPtr<ATestBaseUnit>>& GetTeamArrayConst(ETeam Team) const;

    // '적 기지' 개념: 이제 스포너와 분리.
    // 기본은 '적 성벽(Enemy Team WallAnchor 중 가장 가까운 앵커)'을 목표로 삼는다.
    // 성벽 앵커가 없으면(테스트/예외) 등록된 TeamBase(옵션)로 폴백.
    AActor* GetEnemyBaseFor(const ATestBaseUnit* Unit) const;

    const TArray<TWeakObjectPtr<AActor>>& GetTeamWallAnchorsConst(ETeam Team) const;
    float DistanceToNearestWallAnchorSq(const FVector& P, ETeam WallTeam) const;

    // 후보 선택 (TargetingPolicy 반영)
    ATestBaseUnit* FindBestTargetWithFreeSlot(ATestBaseUnit* Attacker, float Now) const;

    // 예약 실행(원자적으로)
    bool TryReserve(ATestBaseUnit* Attacker, ATestBaseUnit* Target, float Now);
    void CleanupInvalidReferences();
};
