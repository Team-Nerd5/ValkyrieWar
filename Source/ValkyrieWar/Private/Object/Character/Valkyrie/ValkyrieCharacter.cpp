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
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Base/BaseGameplayAbility.h"

#include "Materials/Material.h"
#include "Engine/World.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/AttackModule.h"
#include "Data/Game/AttackData.h"
#include "Data/Attribute/StatAttributeSet.h"

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
	CameraBoom->TargetArmLength = 1200.f;
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

void AValkyrieCharacter::ResetCombo()
{
	CurrentComboCount = 0;
	bIsComboActive = false;
	bIsComboInputOn = false;
	bCanNextCombo = false;
	bIsInComboWindow = false;
}

void AValkyrieCharacter::BeginPlay()
{
	Super::BeginPlay();

}
void AValkyrieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//이거는 캐릭터 정보창에서 장비를 장착해서 변경되었을때, 해당 캐릭터가 로비 캐릭터일 때만 사용
//데이터 업데이트 및 로비에배치된 캐릭터 무기 변경
void AValkyrieCharacter::EquipWeapon(uint64 InValkyrieUID, uint64 InEquipUID)
{
	if (InValkyrieUID != Data->GetUID())
	{
		//장착한 캐릭터가 배치된 캐릭터가 아님
		return;
	}
	//TODO : Inventory에서 Get 해야함. 수정필요
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if (UItemModule* ItemModule = DataManager->GetItemModule())
		{
			//장착한 장비
			EquippedWeapon = ItemModule->GetItem(InEquipUID);

			if (EquippedWeapon && EquippedWeapon->GetItemGroup() != EItemGroup::Equip)
			{
				//장비 아니면 없애버림
				EquippedWeapon = nullptr;
			}
		}
	}

	UpdateWeaponMesh();	
}

void AValkyrieCharacter::UpdateWeaponMesh()
{
	if (!EquippedWeapon)
		return;

	if (EquippedWeapon->IsSkeletalWeapon() && EquippedWeapon->GetSkeletalMesh().IsValid())
	{
		if (SkeletalWeapon)
		{
			SkeletalWeapon->SetSkeletalMesh(EquippedWeapon->GetSkeletalMesh().LoadSynchronous());
			SkeletalWeapon->SetRelativeLocationAndRotation(AttackData->GetLocationOffset(), AttackData->GetRotatinOffset());
		}
	}

	if (!EquippedWeapon->IsSkeletalWeapon() && EquippedWeapon->GetStaticMesh().IsValid())
	{
		if (StaticWeapon)
		{
			StaticWeapon->SetStaticMesh(EquippedWeapon->GetStaticMesh().LoadSynchronous());
			StaticWeapon->SetRelativeLocationAndRotation(AttackData->GetLocationOffset(), AttackData->GetRotatinOffset());
		}
	}
}

void AValkyrieCharacter::ExecuteAttack()
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

void AValkyrieCharacter::OnAttackNotify()
{
	if (ValkyrieMode == EValkyrieModeType::Manual)
	{
		//타겟 찾아서...
		//Projectile이면 생성
		//아니면 타겟에 전달
		ApplyAttack(FindTarget());
	}
	else if (ValkyrieMode == EValkyrieModeType::Auto)
	{
		//AI가 Updatae해준 타겟
		ApplyAttack(CurrentTarget);
	}
}

void AValkyrieCharacter::ExecuteSkill()
{
}

void AValkyrieCharacter::OnSkillNotify()
{
}

void AValkyrieCharacter::SetData(UValkyrieData* InData)
{
	Data = InData;

	StatAttribute->SetAttack(Data->GetStat(EStatusType::Attack));
	StatAttribute->SetDefense(Data->GetStat(EStatusType::Defence));
	StatAttribute->SetHealth(Data->GetStat(EStatusType::Health));
	StatAttribute->SetMaxHealth(Data->GetStat(EStatusType::Health));

	//기본 무기에 따른 공격/스킬 적용
	AttackData = InData->GetAttackData();
	CreateAttackAbility();

	SkillDataList = InData->GetSkillData();
	CreateSkillAbility();

	//캐릭터 블루프린트 생성 후 무기 세팅
	//장비 음..
	UpdateWeaponMesh();

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

AActor* AValkyrieCharacter::FindTarget()
{

	return nullptr;
}


