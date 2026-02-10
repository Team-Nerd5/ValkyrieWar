// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Enums.h"
#include "TestUnitBrainComponent.generated.h"

class ATestBaseUnit;

USTRUCT(BlueprintType)
struct FTestEngagementSlot
{
    GENERATED_BODY()

    // 슬롯에 들어온 공격자
    UPROPERTY()
    TWeakObjectPtr<AActor> Attacker;

    // 슬롯을 확보한 시간 (thrashing 방지/디버그 용)
    UPROPERTY()
    float ReservedAtTime = 0.f;

    bool IsValid() const { return Attacker.IsValid(); }
    void Reset() { Attacker.Reset(); ReservedAtTime = 0.f; }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VALKYRIEWAR_API UTestUnitBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UTestUnitBrainComponent();

    UFUNCTION(BlueprintCallable, Category = "Runtime")
    void ResetRuntimeBrainState();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
    ETeam Team = ETeam::TeamA;

    // 타깃 선택 룰셋(서브시스템은 슬롯/attacker 제한을 유지하고,
    // 여기서는 "어떤 후보를 우선할지"만 결정)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    ETargetingPolicy TargetingPolicy = ETargetingPolicy::Auto;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EReservationState ReservationState = EReservationState::None;

    // 내가 공격하기로 예약한 대상
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TWeakObjectPtr<AActor> ReservedTarget;

    // 내가 목표로 삼는 '적 기지' (기본: 적 성벽 WallAnchor 중 1개)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TWeakObjectPtr<AActor> EnemyBase;

    // 타깃을 얼마나 자주 바꿀지(Thrashing 방지)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tuning")
    float MinHoldSeconds = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
    float LastReservationChangeTime = -1000.f;

public:
    bool CanChangeReservation(float Now) const;
    void SetReservedTarget(AActor* NewTarget, float Now);
    void ClearReservedTarget(float Now);

    UFUNCTION(BlueprintCallable)
    AActor* GetReservedTarget() const { return ReservedTarget.Get(); }

    UFUNCTION(BlueprintCallable)
    ATestBaseUnit* GetOwnerUnit() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
