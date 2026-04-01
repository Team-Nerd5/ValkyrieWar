// Fill out your copyright notice in the D0escription page of Project Settings.


#include "GameSystem/Base/BaseCharacter.h"
#include "GameSystem/Base/BaseGameplayAbility.h"
#include "GameSystem/Base/BaseAnimInstance.h"
#include "GameSystem/Base/BaseProjectile.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"

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

    StatAttributeSet = CreateDefaultSubobject<UStatAttributeSet>(TEXT("CharacterStat"));
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
        if (Spec.GetDynamicSpecSourceTags().HasTagExact(AttackData->GetAbilityTag()))
        {
            AbilitySystemComponent->TriggerAbilityFromGameplayEvent(
                Spec.Handle,
                AbilitySystemComponent->AbilityActorInfo.Get(),
                AttackData->GetAbilityTag(),
                &Payload,
                *AbilitySystemComponent
            );

            TArray<FGameplayCueData> Cues = AttackData->GetCue(EGameplayCueOrder::OnNotify);
            for (const FGameplayCueData Cue : Cues)
            {
                FGameplayCueParameters CueParams;
                CueParams.Location = GetActorLocation() + Cue.Offset;

                AbilitySystemComponent->ExecuteGameplayCue(Cue.Tag, CueParams);
            }

            break;
        }
    }
}

void ABaseCharacter::ApplySkill(int32 InSkillIndex, AActor* InTargetActor)
{
    if (!AbilitySystemComponent) return;

    if (SkillDataList.Num() < (InSkillIndex + 1)) return;

    FGameplayEventData Payload;
    Payload.Instigator = this;
    Payload.Target = InTargetActor;
    Payload.EventTag = SkillDataList[InSkillIndex]->GetAbilityTag();

    //AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);

    for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
    {
        if (Spec.GetDynamicSpecSourceTags().HasTagExact(SkillDataList[InSkillIndex]->GetAbilityTag()))
        {
            AbilitySystemComponent->TriggerAbilityFromGameplayEvent(
                Spec.Handle,
                AbilitySystemComponent->AbilityActorInfo.Get(),
                SkillDataList[InSkillIndex]->GetAbilityTag(),
                &Payload,
                *AbilitySystemComponent
            );

            TArray<FGameplayCueData> Cues = SkillDataList[InSkillIndex]->GetCue(EGameplayCueOrder::OnNotify);
            for (const FGameplayCueData Cue : Cues)
            {
                FGameplayCueParameters CueParams;
                CueParams.Location = GetActorLocation() + Cue.Offset;

                AbilitySystemComponent->ExecuteGameplayCue(Cue.Tag, CueParams);
            }

            break;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("ApplySkill! %s"), *InTargetActor->GetFName().ToString());
}

void ABaseCharacter::UpdateTarget(AActor* InTarget)
{
    CurrentTarget = InTarget;
    TryUseSkillOrAttack();
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

void ABaseCharacter::SetLocomotionBlendSpace()
{
    if (LocomotionBS)
    {
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            if (UBaseAnimInstance* BaseInstance = Cast<UBaseAnimInstance>(AnimInstance))
            {
                BaseInstance->SetInstacne(LocomotionBS, this);
            }
        }
    }
}

void ABaseCharacter::InitProjectilePool()
{
    if (AttackData && AttackData->GetAttackType() == EAttackType::Projectile)
    {
        if (FProjectileDataRow* ProjectileData = AttackData->GetProjectileData())
        {
            if (ProjectileData->EPoolTypes != EPoolTypes::None && !ProjectileData->SpawnObject.IsNull())
            {
                if (UObjectPoolSubsystem* Pool = UGameBaseLibrary::GetObjectPoolSystem(this))
                {
                    UClass* ProjectileClass = ProjectileData->SpawnObject.LoadSynchronous();
                    Pool->InitPool<ABaseProjectile>(ProjectileData->EPoolTypes, ProjectileClass, 10);
                }
            }           
        }
    }

    if (SkillDataList.Num() > 0)
    {
        for (USkillData* SkillData : SkillDataList)
        {
            if (!SkillData || SkillData->GetAttackType() != EAttackType::Projectile)
                continue;

            if (FProjectileDataRow* ProjectileData = SkillData->GetProjectileData())
            {
                if (ProjectileData->EPoolTypes != EPoolTypes::None && !ProjectileData->SpawnObject.IsNull())
                {
                    if (UObjectPoolSubsystem* Pool = UGameBaseLibrary::GetObjectPoolSystem(this))
                    {
                        UClass* ProjectileClass = ProjectileData->SpawnObject.LoadSynchronous();
                        Pool->InitPool<ABaseProjectile>(ProjectileData->EPoolTypes, ProjectileClass, 1);
                    }
                }
            }
        }        
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
            AttackSpec = Spec;
            AbilitySystemComponent->GiveAbility(Spec);
        }
    }
}

void ABaseCharacter::CreateSkillAbility()
{
    if (!AbilitySystemComponent)
    {
        return;
    }

    SkillSpecHandles.Empty();
    SkillSpecHandles.SetNum(SkillDataList.Num());

    for (int32 i = 0; i < SkillDataList.Num(); ++i)
    {
        USkillData* SkillData = SkillDataList[i];
        if (!SkillData)
        {
            continue;
        }

        FGameplayAbilitySpec Spec(UBaseGameplayAbility::StaticClass(), 1, i, SkillData);
        Spec.GetDynamicSpecSourceTags().AddTag(SkillData->GetAbilityTag());

        SkillSpecHandles[i] = AbilitySystemComponent->GiveAbility(Spec);
    }
}
