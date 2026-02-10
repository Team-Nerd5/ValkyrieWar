// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

AValkyrieCharacter::AValkyrieCharacter()
{
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
