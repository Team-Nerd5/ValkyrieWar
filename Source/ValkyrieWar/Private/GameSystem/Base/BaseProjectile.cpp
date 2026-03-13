// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseProjectile.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	MovementComponent->bRotationFollowsVelocity = false;
	MovementComponent->ProjectileGravityScale = 0.0f;

	Effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	Effect->SetupAttachment(RootComponent);
	Effect->SetAutoActivate(false);

	InitialLifeSpan = 3.0f;
}

void ABaseProjectile::OnGet_Implementation()
{
	Effect->SetActive(true);
}

void ABaseProjectile::OnRelease_Implementation()
{
	Effect->SetActive(false);
}

void ABaseProjectile::SetData(FGameplayAbilitySpec InSpec, FProjectileDataRow InProjectileData)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbility(InSpec);
	}
	if (MovementComponent)
	{
		MovementComponent->InitialSpeed = InProjectileData.MoveSpeed;
		MovementComponent->MaxSpeed = InProjectileData.MoveSpeed;
	}
}
