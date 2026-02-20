// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Unit/Animation/UnitAnimInstance.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Unit/Component/UnitBrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUnitAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CacheOwner();
	ResetForReuse();
}

void UUnitAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 에디터/런타임에서 소유자가 바뀌는 경우(풀링/스폰) 대비
	if (!OwnerUnit)
	{
		CacheOwner();
	}

	UpdateFromOwner(DeltaSeconds);
}

void UUnitAnimInstance::CacheOwner()
{
	OwnerUnit = nullptr;
	CachedMoveComp = nullptr;

	APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner) return;

	ACharacter* CharOwner = Cast<ACharacter>(PawnOwner);
	if (!CharOwner) return;

	OwnerUnit = Cast<AUnitCharacter>(CharOwner);
	CachedMoveComp = CharOwner->GetCharacterMovement();
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

	// 방향 계산: “현재 이동 벡터가 어디로 가는지”
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

	// 공격 중 여부: 공격 몽타주 재생중인지로 판단(간단)
	bIsAttacking = IsAnyMontagePlaying();
}

void UUnitAnimInstance::ResetForReuse()
{
	// 풀링 재사용 시 "남아있는 애니 상태" 제거가 핵심
	StopAllMontages(0.f);

	Speed = 0.f;
	Speed2D = 0.f;
	Direction = 0.f;
	bIsInAir = false;
	bIsMoving = false;
	bDead = false;
	bInCombat = false;
	bIsAttacking = false;

	// 캐시 재확인(재사용 시 소유자 바뀌는 케이스 대비)
	CacheOwner();
}
