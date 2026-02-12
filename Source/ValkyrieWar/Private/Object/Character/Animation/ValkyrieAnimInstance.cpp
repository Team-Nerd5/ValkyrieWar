// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/ValkyrieAnimInstance.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UValkyrieAnimInstance::UValkyrieAnimInstance()
{
}

void UValkyrieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<AValkyrieCharacter>(TryGetPawnOwner());
}

void UValkyrieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter == nullptr || MovementComp == nullptr)
	{
		return;
	}

	Velocity = MovementComp->Velocity;
	GroundSpeed = Velocity.Size2D();

	bIsMoving = GroundSpeed > 3.0f;

	FVector CurrentAccel = MovementComp->GetCurrentAcceleration();
	bIsAcceleration = CurrentAccel.SizeSquared() > 0.0f;

	WeaponType = OwnerCharacter->CurrentWeaponType;
}
