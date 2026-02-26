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

USceneComponent* AValkyrieCharacter::GetActiveWeaponComponent()
{
	TArray<USkeletalMeshComponent*> AllSkeletalMeshes;
	this->GetComponents<USkeletalMeshComponent>(AllSkeletalMeshes);

	for (USkeletalMeshComponent* SkelComp : AllSkeletalMeshes)
	{

		if (SkelComp->GetName().Contains(TEXT("WeaponMeshSk")) && SkelComp->GetSkeletalMeshAsset() != nullptr)
		{
			return SkelComp;
		}
	}

	TArray<UStaticMeshComponent*> AllStaticMeshes;
	this->GetComponents<UStaticMeshComponent>(AllStaticMeshes);

	for (UStaticMeshComponent* MeshComp : AllStaticMeshes)
	{
		// 이름이 WeaponMesh 이고, 현재 메쉬가 장착되어 있다면 반환
		if (MeshComp->GetName().Contains(TEXT("WeaponMesh")) && MeshComp->GetStaticMesh() != nullptr)
		{
			return MeshComp;
		}
	}
	return nullptr;
}

void AValkyrieCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AValkyrieCharacter::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
	if (GetController() && IsLocallyControlled())
	{
		if (USceneComponent* WeaponComp = GetActiveWeaponComponent())
		{
			FVector WeaponLoc = WeaponComp->GetComponentLocation();
			FVector SocketLoc = WeaponComp->GetSocketLocation(FName("ArrowSocket"));

			FString DebugMsg = FString::Printf(TEXT("무기: %s \n소켓: %s"), *WeaponLoc.ToString(), *SocketLoc.ToString());
			if (GEngine) GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Cyan, DebugMsg);

			DrawDebugSphere(GetWorld(), SocketLoc, 10.0f, 12, FColor::Red, false, -1.0f);
		}
	}

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
	if (WeaponRow->StaticMesh)
	{
		UStaticMesh* LoadedMesh = WeaponRow->StaticMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			WeaponComp->SetStaticMesh(LoadedMesh);
		}
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


