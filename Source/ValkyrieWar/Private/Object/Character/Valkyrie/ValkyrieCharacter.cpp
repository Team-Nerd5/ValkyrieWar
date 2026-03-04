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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f; // 형 캐릭터 원래 속도로
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

			// 1. 아이템 로드 (동기식으로 가져온다고 가정)
			ItemModule->LoadItem(TestItemUID, TestWeaponDataID, 1);

			// 2. 데이터를 먼저 변수에 확실히 담아!
			EquippedWeapon = ItemModule->GetItem(TestItemUID);

			if (EquippedWeapon && DataManager->GetAttackModule())
			{
				AttackData = DataManager->GetAttackModule()->GetAttackData(EquippedWeapon->GetAttackID());

				// 3. ★데이터가 다 들어온 걸 확인한 이 시점에서 호출!★
				if (AttackData)
				{
					UE_LOG(LogTemp, Warning, TEXT("🏹 [데이터 준비완료] 이제 무기 메쉬 입히러 간다!"));
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
				AttackData = DataManager->GetAttackModule()->GetAttackData(EquippedWeapon->GetAttackID());
				UE_LOG(LogTemp, Warning, TEXT("🗡️ [CD Player] 무기 장착! 몽타주 장전 완료!"));
			}
		}
	}

	UpdateWeaponMesh();	
}

void AValkyrieCharacter::UpdateWeaponMesh()
{
	if (!WeaponClass) {
		UE_LOG(LogTemp, Error, TEXT("❌ WeaponClass 없음!"));
		return;
	}

	// 1. 일단 무기 액터부터 만든다! (데이터 없어도 일단 껍데기라도 소환)
	if (CurrentWeaponActor) { CurrentWeaponActor->Destroy(); }

	FActorSpawnParameters Params;
	Params.Owner = this;
	CurrentWeaponActor = GetWorld()->SpawnActor<AValkyrieWeapon>(WeaponClass, Params);

	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		UE_LOG(LogTemp, Warning, TEXT("✅ 무기 액터 소환 성공! 이제 메쉬 입힌다..."));

		// 2. 그 다음에 데이터를 체크해서 메쉬를 입힌다!
		UItemData* WeaponDataPtr = EquippedWeapon.Get();
		UAttackData* AttackDataPtr = AttackData.Get();

		if (WeaponDataPtr && AttackDataPtr)
		{
			CurrentWeaponActor->SetWeaponMesh(WeaponDataPtr, AttackDataPtr);
			CurrentWeaponActor->SetActorRelativeLocation(AttackDataPtr->GetLocationOffset());
			CurrentWeaponActor->SetActorRelativeRotation(AttackDataPtr->GetRotatinOffset());
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("⚠️ 액터는 만들었는데 메쉬 데이터(ItemData/AttackData)가 아직 없네?"));
		}
	}
}

void AValkyrieCharacter::ExecuteAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("🔘 [키 입력] ExecuteAttack 함수가 드디어 불렸어!!"));

	if (!AttackData) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	UAnimMontage* AttackMontage = AttackData->GetAnimMontage();

	if (AnimInst && AttackMontage)
	{
		// 1. 공격 중이 아닐 때 (1타 쌩으로 시작)
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

			AnimInst->Montage_Play(AttackMontage);
			AnimInst->Montage_JumpToSection(FName("Combo1"), AttackMontage); 
		}
		// 2. 이미 공격 중일 때 (형이 연타하면 무조건 여기로 빠져야 함!)
		else
		{
			bIsComboInputOn = true; // ★ 이게 제일 중요함!! 콤보 예약표 뽑기!
			UE_LOG(LogTemp, Warning, TEXT("🔄 [콤보 예약] 다음 타수 대기 중!"));
		}
	}
}

void AValkyrieCharacter::OnAttackNotify()
{
	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->ExecuteWeaponAction(this, *ArrowClass);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ [에러] 무기 액터가 없어서 쏠 수가 없어!"));
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
	UE_LOG(LogTemp, Warning, TEXT("⏹️ [리셋] 몽타주 종료됨. 이동 제한 해제!"));
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

	// 버튼을 눌러서 예약이 되어있다면?
	if (bIsComboInputOn)
	{
		bIsComboInputOn = false;
		CurrentComboCount++;

		// 다음 타수(Combo2, Combo3 등)로 애니메이션 스무스하게 점프!
		AnimInst->Montage_JumpToSection(NextSectionName, AttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("⏩ [콤보 연결] 다음 섹션(%s)으로 연결!!"), *NextSectionName.ToString());

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
		UE_LOG(LogTemp, Warning, TEXT("⏹️ [콤보 종료] 입력이 없어서 공격 끝!"));
	}
}

AActor* AValkyrieCharacter::FindTarget()
{

	return nullptr;
}


