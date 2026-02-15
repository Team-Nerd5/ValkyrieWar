// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemComponent.h"

#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"

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
	TObjectPtr<UAnimMontage>* FoundMontage = WeaponMontageMap.Find(CurrentWeaponType);
	if (FoundMontage && *FoundMontage)
	{
		PlayAnimMontage(*FoundMontage);
		FString MontageName = (*FoundMontage)->GetName();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Montage Playing: %s"), *MontageName));
		}
	}
}

void AValkyrieCharacter::EquipWeapon(int32 InDataId)
{
	Super::EquipWeapon(InDataId);

	UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();

	if (DataManager)
	{
		USkillData* SkillData = DataManager->GetSkillModule()->GetSkillData(InDataId);

		if (SkillData)
		{
			//공격 Ability 세팅
			UBaseGameplayAbility* NewAbility = NewObject<UBaseGameplayAbility>(this);
			NewAbility->UpdataData(SkillData);

			FGameplayAbilitySpec Spec(NewAbility, 1);
			AbilitySystemComponent->GiveAbility(Spec);
		}
	}
}
