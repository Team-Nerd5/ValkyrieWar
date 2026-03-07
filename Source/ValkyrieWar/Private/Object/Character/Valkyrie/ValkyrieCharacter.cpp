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
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"
#include "Object/Weapon/Range/ValkyrieBow.h"

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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f; 
	}
}

void AValkyrieCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if (UItemModule* ItemModule = DataManager->GetItemModule())
		{
			uint64 TestItemUID = 777;
			int32 TestWeaponDataID = 411101;
			ItemModule->LoadItem(TestItemUID, TestWeaponDataID, 1);

			EquippedWeapon = ItemModule->GetItem(TestItemUID);

			if (EquippedWeapon && DataManager->GetAttackModule())
			{
				AttackData = DataManager->GetAttackModule()->GetAttackData(EquippedWeapon->GetAttackID());
				if (AttackData)
				{
					UpdateWeaponMesh();
				}
			}
		}
	}

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
			if (EquippedWeapon && DataManager->GetAttackModule())
			{
				AttackData = DataManager->GetAttackModule()->GetAttackData(EquippedWeapon->GetAttackID());;
			}
		}
	}

	UpdateWeaponMesh();	
}

void AValkyrieCharacter::UpdateWeaponMesh()
{
	if (!WeaponClass) return;

	// 1. 일단 무기 액터부터 만든다! (데이터 없어도 일단 껍데기라도 소환)
	if (CurrentWeaponActor) { CurrentWeaponActor->Destroy(); }

	FActorSpawnParameters Params;
	Params.Owner = this;
	CurrentWeaponActor = GetWorld()->SpawnActor<AValkyrieWeapon>(WeaponClass, Params);

	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));

		UItemData* WeaponDataPtr = EquippedWeapon.Get();
		UAttackData* AttackDataPtr = AttackData.Get();

		if (WeaponDataPtr && AttackDataPtr)
		{
			CurrentWeaponActor->SetWeaponMesh(WeaponDataPtr, AttackDataPtr);
			CurrentWeaponActor->SetActorRelativeLocation(AttackDataPtr->GetLocationOffset());
			CurrentWeaponActor->SetActorRelativeRotation(AttackDataPtr->GetRotatinOffset());
		}
	}
}

void AValkyrieCharacter::ExecuteAttack()
{

	if (!AttackData) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	UAnimMontage* AttackMontage = AttackData->GetAnimMontage();

	if (AnimInst && AttackMontage)
	{
		if (!AnimInst->Montage_IsPlaying(AttackMontage))
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			GetCharacterMovement()->StopMovementImmediately();

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AValkyrieCharacter::OnAttackMontageEnded);
			AnimInst->Montage_Play(AttackMontage);
			AnimInst->Montage_SetEndDelegate(EndDelegate, AttackMontage);

			CurrentComboCount = 1;
			bIsComboActive = true;
			bCanNextCombo = false;
			bIsComboInputOn = false;

			AnimInst->Montage_JumpToSection(FName("Combo1"), AttackMontage); 
		}
		else
		{
			bIsComboInputOn = true;
		}
	}
}

void AValkyrieCharacter::OnAttackNotify()
{
	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->ExecuteWeaponAction(this, *ArrowClass);
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
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if (UItemModule* ItemModule = DataManager->GetItemModule())
		{
			uint64 TestItemUID = 777;
			int32 TestWeaponDataID = 411101; // 활 ID
			ItemModule->LoadItem(TestItemUID, TestWeaponDataID, 1);
			EquipWeapon(Data->GetUID(), TestItemUID);
		}
	}
	UpdateWeaponMesh();

}
void AValkyrieCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ResetCombo();
}
void AValkyrieCharacter::BeginComboWindow()
{
	bIsInComboWindow = true;
	bCanNextCombo = true;
}

void AValkyrieCharacter::EndComboWindow(FName NextSectionName)
{
	if (!AttackData) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	UAnimMontage* AttackMontage = AttackData->GetAnimMontage();

	if (!AnimInst || !AttackMontage) return;
	if (bIsComboInputOn)
	{
		bIsComboInputOn = false;
		CurrentComboCount++;

		AnimInst->Montage_JumpToSection(NextSectionName, AttackMontage);

		if (NextSectionName == FName("Finish"))
		{
			bIsComboActive = false;
		}
	}
	// 예약된 입력이 없으면 콤보 종료
	else
	{
		CurrentComboCount = 0;
		bIsComboActive = false;
		ResetCombo();
	}
}

//AActor* AValkyrieCharacter::FindTarget()
//{
//
//	return nullptr;
//}
