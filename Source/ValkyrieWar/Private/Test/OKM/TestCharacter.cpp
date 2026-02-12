// Fill out your copyright notice in the Description page of Project Settings.

#include "Test/OKM/TestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "Data/Enum/CommonEnums.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	SubSystem = UGameBaseLibrary::GetObjectPoolSystem(this);
	if (!SubSystem.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Character : 서브시스템이 없습니다"));
		return;
	}
}

void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATestCharacter::Despawn()
{
	SubSystem->Release<ABaseCharacter>(EPoolTypes::BaseCharacter, this);
}

void ATestCharacter::OnGet_Implementation()
{
	GetCapsuleComponent()->SetSimulatePhysics(true);
	//UE_LOG(LogTemp, Log, TEXT("OnSpawnForPool"));
}

void ATestCharacter::OnRelease_Implementation()
{
	GetCapsuleComponent()->SetSimulatePhysics(false);
	//UE_LOG(LogTemp, Log, TEXT("OnReturnToPool"));
}

