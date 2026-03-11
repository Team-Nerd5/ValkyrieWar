// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Weapon/Range/Projectile/ArrowStackComponent.h"
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
#include "Kismet/GameplayStatics.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/AttackModule.h"
#include "Data/Module/SkillModule.h"
#include "Data/Module/SkillEffectModule.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
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
	CurrentComboCount = 0;
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
		GetCharacterMovement()->bOrientRotationToMovement = true;
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

	if (AnimInst && AttackMontage && !AnimInst->Montage_IsPlaying(AttackMontage))
	{
		if (!AnimInst->Montage_IsPlaying(AttackMontage))
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->bOrientRotationToMovement = false;

			// 마우스/입력 방향으로 고개는 돌려주기
			FVector InputDir = GetCharacterMovement()->GetLastInputVector();
			if (!InputDir.IsNearlyZero()) { SetActorRotation(InputDir.Rotation()); }

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AValkyrieCharacter::OnAttackMontageEnded);
			AnimInst->Montage_Play(AttackMontage);
			AnimInst->Montage_SetEndDelegate(EndDelegate, AttackMontage);

			AnimInst->Montage_JumpToSection(FName("Combo1"), AttackMontage);

			CurrentComboCount = 1;
			bIsComboActive = true;
			bCanNextCombo = false;
			bIsComboInputOn = false;
		}
	}
	else
	{
		if (bIsComboActive && bCanNextCombo)
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
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst && AnimInst->IsAnyMontagePlaying()) return; // 중복실행 막기

	if (!EquippedWeapon) return;
	TArray<int32> WeaponSkiilIds = EquippedWeapon->GetSkillID();

	if (WeaponSkiilIds.Num() == 0) return;

	int32 ActiveSkillId = WeaponSkiilIds[0];

	//매니저한테서 모듈 뽑기
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if (USkillModule* SkillModule = DataManager->GetSkillModule())
		{
			TArray<int32> TargetIds = { ActiveSkillId };
			TArray<USkillData*> SkillDataResults = SkillModule->GetSkillData(TargetIds);

			if (SkillDataResults.Num() > 0 && SkillDataResults[0])
			{
				USkillData* SkillData = SkillDataResults[0];
				UAnimMontage* SkillMontage = SkillData->GetMontage().LoadSynchronous();
				float DamageValue = 0.0f;
				for (USkillEffectData* Effect : SkillData->GetEffectList())
				{
					if (Effect)
					{
						// 실제론 Effect->GetValue() 같은 게터 쓰기! 지금은 테스트용 20.0f
						DamageValue = 20.0f;
						break;
					}
				}
				CachedSkillDamage = DamageValue;
				if (SkillMontage && AnimInst)
				{
						//GetCharacterMovement()->MaxWalkSpeed = 0.0f;
						//GetCharacterMovement()->StopMovementImmediately();

						FOnMontageEnded EndDelegate;
						EndDelegate.BindUObject(this, &AValkyrieCharacter::OnSkillMontageEnded);

						AnimInst->Montage_Play(SkillMontage);
						AnimInst->Montage_SetEndDelegate(EndDelegate, SkillMontage);
				}
			}
		}
	}
}

void AValkyrieCharacter::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	ResetCombo();
	UE_LOG(LogTemp, Warning, TEXT("스킬 종료! 이제 다시 움직일 수 있음."));
}

void AValkyrieCharacter::OnSkillNotify()
{
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitCharacter::StaticClass(), FoundEnemies);

	for (AActor* Actor : FoundEnemies)
	{
		if (Actor == this) continue;
		if (UArrowStackComponent* StackComp = Actor->FindComponentByClass<UArrowStackComponent>())
		{
			if (StackComp->StackingArrows.Num() > 0)
			{
				StackComp->PullIt(CachedSkillDamage);
			}
		}
	}
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
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
		FVector InputDir = GetCharacterMovement()->GetLastInputVector();
		if (!InputDir.IsNearlyZero())
		{
			SetActorRotation(InputDir.Rotation());
		}
		if (CurrentComboCount > 1)
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}

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
