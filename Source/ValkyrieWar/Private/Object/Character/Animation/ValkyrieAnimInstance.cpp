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

	OwnerCharacter = Cast<AValkyrieCharacter>(GetOwningActor());

	if (OwnerCharacter)
	{
		MovementComp = OwnerCharacter->GetCharacterMovement();
	}

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

	if (MovementComp && GroundSpeed > 300.0f) // 이속도 이상일때만 출력할거임
	{
		FVector VelocityDir = Velocity.GetSafeNormal(); // 내몸이 향하는 방향

		FVector AccelDir = MovementComp->GetCurrentAcceleration().GetSafeNormal(); // 플레이어가 조작하려는 방향
		float DotResult = FVector::DotProduct(VelocityDir, AccelDir); // 내적 계산

		bShouldPivot = DotResult < -0.5f; // 위 계산의 결과값이 -0.5 이상이면 급선회 발동
		//PlayAnim
	}
	else
	{
		bShouldPivot = false; // 느리면 안할거임
	}
}
