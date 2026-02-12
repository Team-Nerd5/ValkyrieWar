// Fill out your copyright notice in the D0escription page of Project Settings.


#include "GameSystem/Base/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Base/BaseGameplayAbility.h"
#include "Data/Attribute/StatAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;

    StatAttribute = CreateDefaultSubobject<UStatAttributeSet>(TEXT("CharacterStat"));
}

void ABaseCharacter::OnGet_Implementation()
{
}

void ABaseCharacter::OnRelease_Implementation()
{
    AbilitySystemComponent->ClearAllAbilities();
}

void ABaseCharacter::OnConstruction(const FTransform& Transform)
{
    //하위 파츠 세팅
    USkeletalMeshComponent* LeaderMesh = GetMesh();

    TArray<USkeletalMeshComponent*> ModularParts;
    GetComponents<USkeletalMeshComponent>(ModularParts);

    for (USkeletalMeshComponent* Part : ModularParts)
    {
        if (Part == LeaderMesh) continue;

        if (Part->ComponentHasTag(TEXT("Weapon"))) continue;

        Part->SetLeaderPoseComponent(LeaderMesh);
    }
}

void ABaseCharacter::ApplyAttack(AActor* InTargetActor)
{
    if (!AbilitySystemComponent) return;

    FGameplayEventData Payload;
    Payload.Instigator = this;
    Payload.Target = InTargetActor;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackData->GetAbilityTag(), Payload);
}

void ABaseCharacter::ApplySkill(int32 InSkillIndex, AActor* InTargetActor)
{
    if (!AbilitySystemComponent) return;

    if (SkillDataList.Num() > (InSkillIndex + 1)) return;

    FGameplayEventData Payload;
    Payload.Instigator = this;
    Payload.Target = InTargetActor;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, SkillDataList[InSkillIndex]->GetAbilityTag(), Payload);
}

void ABaseCharacter::OnConstruction(const FTransform& Transform)
{
    //하위 파츠 세팅
    USkeletalMeshComponent* LeaderMesh = GetMesh();

    TArray<USkeletalMeshComponent*> ModularParts;
    GetComponents<USkeletalMeshComponent>(ModularParts);

    for (USkeletalMeshComponent* Part : ModularParts)
    {
        if (Part == LeaderMesh) continue;

        if (Part->ComponentHasTag(TEXT("Weapon"))) continue;

        Part->SetLeaderPoseComponent(LeaderMesh);
    }
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}
