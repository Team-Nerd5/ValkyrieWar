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


