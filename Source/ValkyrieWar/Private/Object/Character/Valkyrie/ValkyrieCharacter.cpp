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

#include "Object/Character/Animation/AnimNotifyState/ANS_ComboWindow.h"

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

	// 디버그용
	EquipWeapon(400001); // 활임
}

void AValkyrieCharacter::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
}

void AValkyrieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AValkyrieCharacter::EquipWeapon(uint64 InEquipUID)
{
	UE_LOG(LogTemp, Warning, TEXT("=========== [EquipWeapon 시작] 무기 ID: %llu ==========="), InEquipUID);

	UGameInstance* GameInst = GetGameInstance();
	if (!GameInst)
	{
		return;
	}

	UDataManager* DataManager = GameInst->GetSubsystem<UDataManager>();
	if (!DataManager)
	{
		return;
	}

	UItemModule* ItemModulePtr = DataManager->GetItemModule();
	if (!ItemModulePtr)
	{
		return;
	}

	// 여기서 걸리면 Init()에서 엑셀을 못 읽었거나 400001번이 없는 거임!
	const FItemDataRow* WeaponRow = ItemModulePtr->GetTableDataById(static_cast<int32>(InEquipUID));
	if (!WeaponRow)
	{
		
		return;
	}
	UStaticMeshComponent* WeaponComp = nullptr;
	TArray<UStaticMeshComponent*> AllSkeletalMeshes;
	GetComponents<UStaticMeshComponent>(AllSkeletalMeshes);

	for (UStaticMeshComponent* MeshComp : AllSkeletalMeshes)
	{
		if (MeshComp->GetName().Contains(TEXT("WeaponMesh")))
		{
			WeaponComp = MeshComp;
			break;
		}
	}
	if (!WeaponComp)
	{
		return;
	}
	UStaticMesh* LoadedMesh = WeaponRow->Mesh.LoadSynchronous();
	if (LoadedMesh)
	{
		WeaponComp->SetStaticMesh(LoadedMesh);
		UE_LOG(LogTemp, Warning, TEXT("🎉 [최종 성공] %llu번 무기 메쉬 장착 완료!!! 🎉"), InEquipUID);
	}

	if (WeaponRow->AttackId > 0)
	{
		// 데이터 행 빼오기
		UAttackModule* AttackModulePtr = DataManager->GetAttackModule();

		if (AttackModulePtr)
		{
			UAttackData* AttackObj = AttackModulePtr->GetAttackData(WeaponRow->AttackId);
			if (AttackObj)
			{
				this->ComboMontage = AttackObj->GetAnimMontage();
			}
		}
	}
	
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

	// 1. 완전 처음 1타 시작 (몽타주 안 도는 중)
	if (!AnimInst->Montage_IsPlaying(ComboMontage))
	{
		// 🚨 내가 빼먹었던 거 복구 & 형의 커스텀 변수 ON!!
		CurrentComboCount = 1;
		bIsComboActive = true; // (형 코드에 이 변수 있으면 무조건 true로 켜줘야 해!)

		bCanNextCombo = false;
		bIsComboInputOn = false;

		AnimInst->Montage_Play(ComboMontage);
		AnimInst->Montage_JumpToSection(FName("Combo1"), ComboMontage);

		UE_LOG(LogTemp, Warning, TEXT("⚔️ 콤보 시작! 1타 발사"));
	}
	else
	{
		bIsComboInputOn = true;
		UE_LOG(LogTemp, Warning, TEXT("📩 예약 완료! (현재 타수: %d)"), CurrentComboCount);
	}
}

void AValkyrieCharacter::BeginComboWindow()
{
	bIsInComboWindow = true;
	bCanNextCombo = true;
	UE_LOG(LogTemp, Warning, TEXT("🟢 [BeginComboWindow] 호출됨 - ComboCount: %d, bIsComboActive: %d, bIsComboInputOn: %d"),
		CurrentComboCount, bCanNextCombo, bIsComboInputOn);
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

		// 🎯 다음 콤보로 점프!!
		AnimInst->Montage_JumpToSection(NextSectionName, ComboMontage);
		UE_LOG(LogTemp, Warning, TEXT("➡️ 다음 콤보 발사: %s"), *NextSectionName.ToString());

		if (NextSectionName == FName("Finish"))
		{
			CurrentComboCount = 0;
			// bIsComboActive = false; // 피니시 나갈 때 꺼줘도 됨
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ 클릭 안함! 콤보 완전 종료"));
		CurrentComboCount = 0;
		bIsComboActive = false; // 🚨 콤보 끊기면 State Machine도 대기 상태로!
	}
}


