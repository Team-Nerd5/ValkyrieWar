// Fill out your copyright notice in the Description page of Project Settings.

#include "Test/OKM/TestActorSpawner.h"
#include "Data/Pool/PoolTypes.h"
#include "Data/Enums.h"
#include "GameSystem/Base/BaseCharacter.h"

// Sets default values
ATestActorSpawner::ATestActorSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ATestActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	SubSystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
	if (!SubSystem.IsValid())
		return;

	SubSystem->InitPool<ABaseCharacter>(EPoolTypes::BaseCharacter, TestCharacter, 1000);

}

void ATestActorSpawner::SpawnActor()
{
	SpawnedActor = SubSystem->Get<ABaseCharacter>(EPoolTypes::BaseCharacter, TestCharacter, GetActorLocation(), GetActorRotation());

}

void ATestActorSpawner::DespawnActor()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle2);

	if (!SpawnedActor.IsValid())
		return;

	// 풀로 반환했으면 로컬 참조 해제
	SpawnedActor = nullptr;
}

