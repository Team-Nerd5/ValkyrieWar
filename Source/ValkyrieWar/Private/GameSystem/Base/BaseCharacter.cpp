// Fill out your copyright notice in the D0escription page of Project Settings.


#include "GameSystem/Base/BaseCharacter.h"
#include "GameSystem/Base/BaseGameplayAbility.h"

#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Attribute/StatAttributeSet.h"
#include "Components/CapsuleComponent.h"

#include "AbilitySystemComponent.h"
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

        if (Part->ComponentHasTag(TEXT("Weapon")))
        {
            SkeletalWeapon = Part;
            continue;
        }

        Part->SetLeaderPoseComponent(LeaderMesh);
    }

    TArray<UStaticMeshComponent*> StaticParts;
    GetComponents<UStaticMeshComponent>(StaticParts);

    for (UStaticMeshComponent* Part : StaticParts)
    {

        if (Part->ComponentHasTag(TEXT("Weapon")))
        {
            StaticWeapon = Part;
            break;
        }
    }
}

void ABaseCharacter::ApplyAttack(AActor* InTargetActor)
{
    if (!AbilitySystemComponent || !AttackData) return;

    FGameplayEventData Payload;
    Payload.Instigator = this;
    Payload.Target = InTargetActor;

    for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
    {
        // Spec에 우리가 부여했던 태그가 있는지 확인
        if (Spec.GetDynamicSpecSourceTags().HasTagExact(AttackData->GetAbilityTag()))
        {
            AbilitySystemComponent->TriggerAbilityFromGameplayEvent(
                Spec.Handle,
                AbilitySystemComponent->AbilityActorInfo.Get(),
                AttackData->GetAbilityTag(),
                &Payload,
                *AbilitySystemComponent
            );
            break;
        }
    }
}

void ABaseCharacter::ApplySkill(int32 InSkillIndex, AActor* InTargetActor)
{
    if (!AbilitySystemComponent) return;

    if (SkillDataList.Num() > (InSkillIndex + 1)) return;

    FGameplayEventData Payload;
    Payload.Instigator = this;
    Payload.Target = InTargetActor;
    Payload.EventTag = SkillDataList[InSkillIndex]->GetAbilityTag();

    AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void ABaseCharacter::UpdateTarget(AActor* InTarget)
{
    CurrentTarget = InTarget;
    ExecuteAttack();
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ABaseCharacter::CreateAttackAbility()
{
    if (AttackData)
    {
        if (AttackData->GetAbilityTag().IsValid())
        {
            FGameplayAbilitySpec Spec(UBaseGameplayAbility::StaticClass(), 1, -1, AttackData);
            Spec.GetDynamicSpecSourceTags().AddTag(AttackData->GetAbilityTag());
            AbilitySystemComponent->GiveAbility(Spec);
        }
    }
}

void ABaseCharacter::CreateSkillAbility()
{
    if (SkillDataList.Num() > 0)
    {
        for (USkillData* SkillData : SkillDataList)
        {
            
        }
    }
}
