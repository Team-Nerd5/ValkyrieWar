// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemComponent.h"

#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Base/BaseGameplayAbility.h"

#include "Materials/Material.h"
#include "Engine/World.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/AttackModule.h"
#include "Data/Game/AttackData.h"

#include "Object/Character/Valkyrie/Animation/AnimNotifyState/ANS_ComboWindow.h"

AValkyrieCharacter::AValkyrieCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	PrimaryActorTick.bCanEverTick = true;
}

void AValkyrieCharacter::SetWeaponType(EWeaponAnimType InNewType)
{
	CurrentWeaponType = InNewType;
}

void AValkyrieCharacter::BeginPlay()
{
	Super::BeginPlay();

}
void AValkyrieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AValkyrieCharacter::EquipWeapon(uint64 InEquipUID)
{
	//TODO : Inventory에서 Get 해야함. 수정필요
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if (UItemModule* ItemModule = DataManager->GetItemModule())
		{
			UItemData* EquipItem = ItemModule->GetItem(InEquipUID);

			if (!EquipItem || EquipItem->GetItemGroup() != EItemGroup::Equip)
			{
				//장비가 아님
				return;
			}

			if (EquipItem->IsSkeletalWeapon() && EquipItem->GetSkeletalMesh().IsValid())
			{
				if (SkeletalWeapon)
				{
					SkeletalWeapon->SetSkeletalMesh(EquipItem->GetSkeletalMesh().LoadSynchronous());
				}
			}

			if (!EquipItem->IsSkeletalWeapon() && EquipItem->GetStaticMesh().IsValid())
			{
				if (StaticWeapon)
				{
					StaticWeapon->SetStaticMesh(EquipItem->GetStaticMesh().LoadSynchronous());
				}
			}
		}
	}

	AttackData = Data->GetAttackData();
	SkillDataList = Data->GetSkillData();

	ComboMontage = AttackData->GetAnimMontage();
	//TODO : AnimInstance 바꾸는거 확인 필요
}

void AValkyrieCharacter::SetData(UValkyrieData* InData)
{
	Data = InData;

	//기본 무기에 따른 공격/스킬 적용
	AttackData = InData->GetAttackData();

	SkillDataList = InData->GetSkillData();
}

// 기본공격 콤보 노티파이 스테이트
void AValkyrieCharacter::Attack()
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst || !ComboMontage) return;

	
	if (!AnimInst->Montage_IsPlaying(ComboMontage))
	{
		
		CurrentComboCount = 1;
		bIsComboActive = true; 

		bCanNextCombo = false;
		bIsComboInputOn = false;

		AnimInst->Montage_Play(ComboMontage);
		AnimInst->Montage_JumpToSection(FName("Combo1"), ComboMontage);
	}
	else
	{
		bIsComboInputOn = true;
	}
}

void AValkyrieCharacter::BeginComboWindow()
{
	bIsInComboWindow = true;
	bCanNextCombo = true;
}

void AValkyrieCharacter::EndComboWindow(FName NextSectionName)
{
	if (!bCanNextCombo) return;

	bCanNextCombo = false;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst) return;

	if (bIsComboInputOn)
	{
		bIsComboInputOn = false;
		CurrentComboCount++;

		AnimInst->Montage_JumpToSection(NextSectionName, ComboMontage);

		if (NextSectionName == FName("Finish"))
		{
			CurrentComboCount = 0;
		}
	}
	else
	{
		CurrentComboCount = 0;
		bIsComboActive = false; 
	}
}


