// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseProjectile.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

ABaseProjectile::ABaseProjectile()
{

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	Effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	Effect->SetupAttachment(RootComponent);
}

void ABaseProjectile::OnGet_Implementation()
{
}

void ABaseProjectile::OnRelease_Implementation()
{
}
