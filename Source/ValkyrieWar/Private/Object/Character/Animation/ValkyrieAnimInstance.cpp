// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/ValkyrieAnimInstance.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

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

	if (OwnerCharacter)
	{
		WeaponType = OwnerCharacter->CurrentWeaponType;
	}
	if (OwnerCharacter == nullptr)
	{
		return;
	}
	Velocity = OwnerCharacter->GetVelocity();
	GroundSpeed = Velocity.Size2D();
	bIsMoving = GroundSpeed > 3.0f;

	WeaponType = OwnerCharacter->CurrentWeaponType;
}
