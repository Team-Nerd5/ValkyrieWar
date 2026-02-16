// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Base/BaseGameplayAbility.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;
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

void ABaseCharacter::EquipWeapon(int32 InDataId)
{
    //공격 데이터를 가져옴
    UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();

    if (DataManager)
    {
        EquippedWeapon = DataManager->GetItemModule()->GetTableDataById(InDataId);
        if (EquippedWeapon && EquippedWeapon->AttackId > 0)
        {
            UAttackData* AttackData = DataManager->GetAttackModule()->GetAttackData(EquippedWeapon->AttackId);

            if (AttackData)
            {
                //공격 Ability 세팅
                UBaseGameplayAbility* NewAbility = NewObject<UBaseGameplayAbility>(this);
                NewAbility->UpdateData(AttackData->GetEffectList());

                FGameplayAbilitySpec Spec(NewAbility, 1);
                AbilitySystemComponent->GiveAbility(Spec);

                //애니메이션 등 세팅
            }
        }
       
    }
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}
