// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseAnimInstance.h"

void UBaseAnimInstance::SetInstacne(UBlendSpace* InBlendSpace, ABaseCharacter* InOwner)
{
	InitAnimData();

	if (!InOwner)
		return;

	Owner = InOwner;

	if (InBlendSpace)
	{
		LocomotionBS = InBlendSpace;
	}
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	InitAnimData();
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Owner)
	{
		FVector Velocity = Owner->GetVelocity();
		MoveSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	}
}

//유닛은 풀링되니까 초기화 필요
void UBaseAnimInstance::InitAnimData()
{
	MoveSpeed = 0.0f;

	StopAllMontages(0.f);

	Owner = nullptr;

	LocomotionBS = DefaultBlendSpace.LoadSynchronous();
}
