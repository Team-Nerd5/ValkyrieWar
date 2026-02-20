// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Enum/CommonEnums.h"
#include "UnitBrainComponent.generated.h"

class AUnitCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VALKYRIEWAR_API UUnitBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitBrainComponent();

    UFUNCTION(BlueprintCallable, Category = "Runtime")
    void ResetRuntimeBrainState();

public:
    bool CanChangeReservation(float Now) const;
    void SetReservedTarget(AActor* NewTarget, float Now);
    void ClearReservedTarget(float Now);

    UFUNCTION(BlueprintCallable)
    AActor* GetReservedTarget() const { return ReservedTarget.Get(); }

    UFUNCTION(BlueprintCallable)
    AUnitCharacter* GetOwnerUnit() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
};
