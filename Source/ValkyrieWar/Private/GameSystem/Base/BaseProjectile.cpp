// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseProjectile.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Data/Attribute/StatAttributeSet.h"

#include "NiagaraComponent.h"
#include <Object/Actor/Wall/ProjectileWallActor.h>

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilityComponent"));

	StatAttributeSet = CreateDefaultSubobject<UStatAttributeSet>(TEXT("ProjectileStat"));

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));

	SetRootComponent(Collision);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->ProjectileGravityScale = 0.0f;
	MovementComponent->bInitialVelocityInLocalSpace = true;

	MovementComponent->SetUpdatedComponent(RootComponent);

	Effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	Effect->SetupAttachment(RootComponent);
	Effect->SetAutoActivate(false);

	InitialLifeSpan = .5f;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();	
}

void ABaseProjectile::OnGet_Implementation()
{
	Collision->OnComponentBeginOverlap.Clear();
	MovementComponent->Velocity = FVector::ZeroVector;
	MovementComponent->UpdateComponentVelocity();
}

void ABaseProjectile::OnRelease_Implementation()
{
	TeamType = ETeamType::None;
	Effect->Deactivate();
	MovementComponent->StopMovementImmediately();
	MovementComponent->Deactivate();
}

void ABaseProjectile::SetData(FGameplayTag InTag, FGameplayAbilitySpec InSpec, FProjectileDataRow InProjectileData, TArray<FGameplayCueData> InCues)
{
	if (AbilitySystemComponent)
	{		
		AbilitySystemComponent->GiveAbility(InSpec);
		AbilityTag = InTag;
	}
	PoolType = InProjectileData.EPoolTypes;

	if (MovementComponent)
	{
		MovementComponent->InitialSpeed = InProjectileData.MoveSpeed;
		MovementComponent->MaxSpeed = InProjectileData.MoveSpeed;

		MovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * InProjectileData.MoveSpeed);
		MovementComponent->Activate();
	}
	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);
	}

	Effect->ResetSystem();
	Effect->Activate(true);

	StatAttributeSet->SetHealth(0.0f);
	StatAttributeSet->SetMaxHealth(0.0f);
	StatAttributeSet->SetDefense(0.0f);

	PlayCueOnTarget = InCues;
}

//
void ABaseProjectile::SetAttack(float InAttack)
{
	StatAttributeSet->SetAttack(InAttack);
}

void ABaseProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AActor* TargetActor = nullptr;

	UE_LOG(LogTemp, Log, TEXT("OnOverlap : %s"), *OtherActor->GetFName().ToString());

	if (ABaseCharacter* TargetUnit = Cast<ABaseCharacter>(OtherActor))
	{
		//현재 상태...스폰되자마자 자신이랑 충돌됨. 팀타입이 None이니까 충돌처리되서 제거됨...
		if (TargetUnit->IsDead() || TargetUnit->GetTeamType() == TeamType)
		{
			return;
		}

		TargetActor = TargetUnit;
	}
	else if (AProjectileWallActor* TargetWall = Cast<AProjectileWallActor>(OtherActor))
	{
		if (TargetWall->GetTeamType() == TeamType)
		{
			return;
		}

		TargetActor = TargetWall;
	}
	else
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = this;
	Payload.Target = TargetActor;

	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(AbilityTag))
		{
			AbilitySystemComponent->TriggerAbilityFromGameplayEvent(
				Spec.Handle,
				AbilitySystemComponent->AbilityActorInfo.Get(),
				AbilityTag,
				&Payload,
				*AbilitySystemComponent
			);
			break;
		}
	}

	if (PlayCueOnTarget.Num() > 0)
	{
		for (FGameplayCueData& Cue : PlayCueOnTarget)
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = TargetActor->GetActorLocation();

			AbilitySystemComponent->ExecuteGameplayCue(Cue.Tag, CueParams);
		}
	}

	if (UObjectPoolSubsystem* Pool = UGameBaseLibrary::GetObjectPoolSystem(this))
	{
		Pool->Release(PoolType, this);
	}
}
