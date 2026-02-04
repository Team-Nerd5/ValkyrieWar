// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

AValkyrieCharacter::AValkyrieCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AValkyrieCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AValkyrieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


