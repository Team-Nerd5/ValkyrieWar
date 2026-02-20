// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Unit/Component/UnitBrainComponent.h"
#include "Object/Character/Unit/UnitCharacter.h"

UUnitBrainComponent::UUnitBrainComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UUnitBrainComponent::ResetRuntimeBrainState()
{
    ReservedTarget.Reset();
    EnemyBase.Reset();
    ReservationState = EReservationState::None;
    LastReservationChangeTime = -1000.f;
}

void UUnitBrainComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UUnitBrainComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 서브시스템이 정리하는 게 원칙이지만, 안전장치로 여기서도 비움
    ReservedTarget.Reset();
    ReservationState = EReservationState::None;
    LastReservationChangeTime = -1000.f;

    Super::EndPlay(EndPlayReason);
}

bool UUnitBrainComponent::CanChangeReservation(float Now) const
{
    return (Now - LastReservationChangeTime) >= MinHoldSeconds;
}

void UUnitBrainComponent::SetReservedTarget(AActor* NewTarget, float Now)
{
    ReservedTarget = NewTarget;
    LastReservationChangeTime = Now;
    ReservationState = (NewTarget ? EReservationState::Reserved : EReservationState::None);
}

void UUnitBrainComponent::ClearReservedTarget(float Now)
{
    ReservedTarget.Reset();
    LastReservationChangeTime = Now;
    ReservationState = EReservationState::None;
}

AUnitCharacter* UUnitBrainComponent::GetOwnerUnit() const
{
    return Cast<AUnitCharacter>(GetOwner());
}

