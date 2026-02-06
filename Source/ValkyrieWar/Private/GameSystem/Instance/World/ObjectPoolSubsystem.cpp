// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "GameSystem/Base/BasePawn.h"
#include "GameSystem/Base/BaseActor.h"
#include "Interface/ObjectPool/ObjectPoolInterface.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ObjectPoolMap.Empty();
}
