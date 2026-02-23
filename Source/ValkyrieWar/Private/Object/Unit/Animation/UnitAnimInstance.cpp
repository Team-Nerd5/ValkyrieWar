// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Unit/Animation/UnitAnimInstance.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Unit/Component/UnitBrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUnitAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 초기값: 아직 병과를 못 읽는 경우를 대비해 Default를 우선 세팅
	LocomotionBS_Current = LocomotionBS_Default;

	CachedPawnOwner = nullptr;

	CacheOwner();
	ApplyLocomotionBlendSpace();
	ResetForReuse();
}

void UUnitAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 풀링/스폰 등으로 PawnOwner가 바뀌면 즉시 다시 캐시 + BS 재적용
	APawn* PawnOwner = TryGetPawnOwner();
	const bool bOwnerChanged = (CachedPawnOwner.Get() != PawnOwner);

	if (!OwnerUnit || bOwnerChanged)
	{
		CacheOwner();
		ApplyLocomotionBlendSpace();
	}

	UpdateFromOwner(DeltaSeconds);
}

void UUnitAnimInstance::CacheOwner()
{
	OwnerUnit = nullptr;
	CachedMoveComp = nullptr;

	APawn* PawnOwner = TryGetPawnOwner();
	CachedPawnOwner = PawnOwner;
	if (!PawnOwner) return;

	ACharacter* CharOwner = Cast<ACharacter>(PawnOwner);
	if (!CharOwner) return;

	OwnerUnit = Cast<AUnitCharacter>(CharOwner);
	CachedMoveComp = CharOwner->GetCharacterMovement();
}

void UUnitAnimInstance::ApplyLocomotionBlendSpace()
{
	if (!OwnerUnit)
	{
		LocomotionBS_Current = LocomotionBS_Default;
		EnsureLocomotionBSValid();
		return;
	}

	UBlendSpace* NewBS = OwnerUnit->GetLocomotionBS();

	// nullptr이면 Default
	if (!NewBS)
	{
		NewBS = LocomotionBS_Default;
	}

	// 이미 같은 BS면 재적용 불필요
	if (LocomotionBS_Current == NewBS)
	{
		return;
	}

	LocomotionBS_Current = NewBS;
}

void UUnitAnimInstance::EnsureLocomotionBSValid()
{
	// 매핑 누락/실수 대비: Current가 비어있으면 Default로
	if (!LocomotionBS_Current)
	{
		LocomotionBS_Current = LocomotionBS_Default;
	}
	// Default도 없다면 그냥 nullptr 상태로 남는데,
	// 이 경우 ABP에서 Asset이 null이면 포즈가 깨질 수 있으니
	// 가능하면 Default는 반드시 지정.
}

void UUnitAnimInstance::UpdateFromOwner(float DeltaSeconds)
{
	if (!OwnerUnit)
	{
		Speed = 0.f;
		Speed2D = 0.f;
		Direction = 0.f;
		bIsInAir = false;
		bIsMoving = false;
		bDead = false;
		bInCombat = false;
		bIsAttacking = false;
		return;
	}

	const FVector Vel = OwnerUnit->GetVelocity();
	Speed = Vel.Size();
	Speed2D = FVector(Vel.X, Vel.Y, 0.f).Size();

	const FRotator ActorRot = OwnerUnit->GetActorRotation();
	FVector Forward = OwnerUnit->GetActorForwardVector();
	FVector Right = OwnerUnit->GetActorRightVector();
	FVector NormalizedVel = Vel.GetSafeNormal2D();

	float ForwardDot = FVector::DotProduct(Forward, NormalizedVel);
	float RightDot = FVector::DotProduct(Right, NormalizedVel);
	Direction = FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));

	bDead = OwnerUnit->IsDead();

	if (CachedMoveComp)
	{
		bIsInAir = CachedMoveComp->IsFalling();
	}
	else
	{
		bIsInAir = false;
	}

	bIsMoving = (Speed2D > 3.f) && !bIsInAir && !bDead;

	// 전투 여부(ReservedTarget 기반) - 필요에 따라 정책 변경 가능
	if (OwnerUnit->Brain)
	{
		bInCombat = OwnerUnit->Brain->ReservedTarget.IsValid();
	}
	else
	{
		bInCombat = false;
	}

	bIsAttacking = IsAnyMontagePlaying();
}

void UUnitAnimInstance::ResetForReuse()
{
	StopAllMontages(0.f);

	Speed = 0.f;
	Speed2D = 0.f;
	Direction = 0.f;
	bIsInAir = false;
	bIsMoving = false;
	bDead = false;
	bInCombat = false;
	bIsAttacking = false;

	// 재사용 시점에 Owner가 바뀌어 있을 수 있으니 다시 캐시 + BS 적용
	CacheOwner();
	ApplyLocomotionBlendSpace();
}
