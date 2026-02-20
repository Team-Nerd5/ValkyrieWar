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

void AValkyrieCharacter::DoLightAttack()
{
	if (bIsAttacking)
	{
		if (ComboCount < MaxComboCount)
		{
			bSaveAttack = true;
		}
	}
	else
	{
		bIsAttacking = true;
		ComboCount = 1;

		if (AbilitySystemComponent)
		{
			FGameplayTag MyTestTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack"));
			FGameplayTagContainer TagContainer(MyTestTag);
			AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
		}
	}
}

void AValkyrieCharacter::DoHeavyAttack()
{
	bSaveAttack = false; // 강공격 커맨드 들어오면 약공격 예약 다 찌부시켜버리고 강공격 우선
	ResetCombo();
	ApplySkill(0, nullptr);
}

void AValkyrieCharacter::ContinueCombo()
{

	if (bSaveAttack)
	{
		bSaveAttack = false;
		ComboCount++;

		if (AbilitySystemComponent)
		{
			FGameplayTag MyTestTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack"));
			FGameplayTagContainer TagContainer(MyTestTag);
			bool bSuccess = AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
		}
	}
	else
	{

		bIsAttacking = false;
		ComboCount = 0;
	}
}

void AValkyrieCharacter::ResetCombo() // 콤보끝나면 싹 초기화
{
	bIsAttacking = false;

	bSaveAttack = false;
	ComboCount = 0;
}

void AValkyrieCharacter::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
}

void AValkyrieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AValkyrieCharacter::Attack()
{
	if (AttackData)
	{
		UAnimMontage* MontageToPlay = AttackData->GetAnimMontage();

		if (MontageToPlay)
		{
			PlayAnimMontage(MontageToPlay);
		}
	}
}

void AValkyrieCharacter::EquipWeapon(uint64 InEquipUID)
{
	UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();

	if (DataManager)
	{
		EquippedWeapon = DataManager->GetItemModule()->GetItem(InEquipUID);
		if (EquippedWeapon)
		{
			EquippedWeapon->Equip(Data->GetUID());

			TArray<USkillData*> SkillData = DataManager->GetSkillModule()->GetSkillData(EquippedWeapon->GetSkillID());

			Data->UpdateWeapon(EquippedWeapon, GetGameInstance<UGameManager>());

			AttackData = Data->GetAttackData();
			SkillDataList = Data->GetSkillData();
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
