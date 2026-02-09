// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestComponents/TestUnitBrainComponent.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"

UTestUnitBrainComponent::UTestUnitBrainComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTestUnitBrainComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UTestUnitBrainComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 서브시스템이 정리하는 게 원칙이지만, 안전장치로 여기서도 비움
    ReservedTarget.Reset();
    Super::EndPlay(EndPlayReason);
}

bool UTestUnitBrainComponent::CanChangeReservation(float Now) const
{
    return (Now - LastReservationChangeTime) >= MinHoldSeconds;
}

void UTestUnitBrainComponent::SetReservedTarget(AActor* NewTarget, float Now)
{
    ReservedTarget = NewTarget;
    LastReservationChangeTime = Now;
    ReservationState = (NewTarget ? EReservationState::Reserved : EReservationState::None);
}

void UTestUnitBrainComponent::ClearReservedTarget(float Now)
{
    ReservedTarget.Reset();
    LastReservationChangeTime = Now;
    ReservationState = EReservationState::None;
}

ATestBaseUnit* UTestUnitBrainComponent::GetOwnerUnit() const
{
    return Cast<ATestBaseUnit>(GetOwner());
}
