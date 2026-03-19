// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseProjectile.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "NiagaraComponent.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	Collision->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	SetRootComponent(Collision);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->ProjectileGravityScale = 0.0f;
	MovementComponent->bInitialVelocityInLocalSpace = true;

	MovementComponent->SetUpdatedComponent(RootComponent);

	Effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	Effect->SetupAttachment(RootComponent);
	Effect->SetAutoActivate(false);

	InitialLifeSpan = 3.0f;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);
	}
}

void ABaseProjectile::OnGet_Implementation()
{
	Effect->SetActive(true);
}

void ABaseProjectile::OnRelease_Implementation()
{
	Effect->SetActive(false);
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

	PlayCueOnTarget = InCues;
}

void ABaseProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AUnitCharacter* TargetUnit = Cast<AUnitCharacter>(OtherActor);
	if (!TargetUnit || TargetUnit->IsDead() || TargetUnit->GetTeamType() != ETeamType::Enemy) return;

	FGameplayEventData Payload;
	Payload.Instigator = this;
	Payload.Target = TargetUnit;

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
		for (FGameplayCueData Cue : PlayCueOnTarget)
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = TargetUnit->GetActorLocation();

			AbilitySystemComponent->ExecuteGameplayCue(Cue.Tag, CueParams);
		}
	}

	if (UObjectPoolSubsystem* Pool = UGameBaseLibrary::GetObjectPoolSystem(this))
	{
		Pool->Release(PoolType, this);
	}
}
