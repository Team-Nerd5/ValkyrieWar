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

void ABaseCharacter::UpdateTarget(AActor* InTarget)
{
    CurrentTarget = InTarget;
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
        UBaseGameplayAbility* AttackAbility = NewObject<UBaseGameplayAbility>(this);
        AttackAbility->UpdateData(AttackData->GetAbilityTag(), AttackData->GetEffectList());
        FGameplayAbilitySpec Spec(AttackAbility, 1);
        AbilitySystemComponent->GiveAbility(Spec);
    }
}

void ABaseCharacter::CreateSkillAbility()
{
    if (SkillDataList.Num() > 0)
    {
        for (USkillData* SkillData : SkillDataList)
        {
            UBaseGameplayAbility* SkillAbility = NewObject<UBaseGameplayAbility>(this);
            SkillAbility->UpdateData(SkillData->GetAbilityTag(), SkillData->GetEffectList());

            //레벨은 나중에 생각하자...
            FGameplayAbilitySpec Spec(SkillAbility, 1);
            AbilitySystemComponent->GiveAbility(Spec);
        }
    }
}
