// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestActors/TestUnitSpawner.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"

ATestUnitSpawner::ATestUnitSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATestUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	UObjectPoolSubsystem* Pool = GetPoolSubsystem();
	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("[TestUnitSpawner] ObjectPoolSubsystem not found"));
		return;
	}

	// 풀 초기화(Reserve)
	for (const FTestPoolSpawnEntry& Entry : PoolEntries)
	{
		if (!Entry.UnitClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[TestUnitSpawner] PoolEntry has no UnitClass"));
			continue;
		}

		// 템플릿 타입은 반드시 실제 클래스와 맞아야 함
		Pool->InitPool<ABaseCharacter>(Entry.PoolType, Entry.UnitClass, Entry.ReserveSize);
	}
}

const FTestPoolSpawnEntry* ATestUnitSpawner::FindEntry(EPoolTypes PoolType) const
{
	for (const FTestPoolSpawnEntry& E : PoolEntries)
	{
		if (E.PoolType == PoolType)
		{
			return &E;
		}
	}
	return nullptr;
}

ABaseCharacter* ATestUnitSpawner::SpawnFromPool(EPoolTypes PoolType)
{
	UObjectPoolSubsystem* Pool = GetPoolSubsystem();
	if (!Pool) return nullptr;

	const FTestPoolSpawnEntry* Entry = FindEntry(PoolType);
	if (!Entry || !Entry->UnitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[TestUnitSpawner] SpawnFromPool failed. No entry for PoolType"));
		return nullptr;
	}

	const FTransform SpawnTM = GetSpawnTransform();
	const FVector Loc = SpawnTM.GetLocation();
	const FRotator Rot = SpawnTM.Rotator();

	ABaseCharacter* Unit = Pool->Get<ABaseCharacter>(PoolType, Entry->UnitClass, Loc, Rot);
	if (!Unit)
	{
		UE_LOG(LogTemp, Error, TEXT("[TestUnitSpawner] Pool->Get returned null"));
		return nullptr;
	}

	// 스폰 직후 초기화 훅(여기서 Team 설정, BT 재시작, 브레인 리셋 등을 해주면 안정적)
	BP_OnUnitSpawned(Unit);
	return Unit;
}

void ATestUnitSpawner::DespawnToPool(EPoolTypes PoolType, ABaseCharacter* Unit)
{
	if (!Unit) return;

	UObjectPoolSubsystem* Pool = GetPoolSubsystem();
	if (!Pool)
	{
		Unit->Destroy();
		return;
	}

	BP_OnUnitDespawned(Unit);

	// 반드시 “스폰에 사용했던 PoolType”으로 반환해야 함
	Pool->Release<ABaseCharacter>(PoolType, Unit);
}

UObjectPoolSubsystem* ATestUnitSpawner::GetPoolSubsystem() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UObjectPoolSubsystem>() : nullptr;
}

FTransform ATestUnitSpawner::GetSpawnTransform() const
{
	const FTransform BaseTM = SpawnPointActor ? SpawnPointActor->GetActorTransform() : GetActorTransform();
	if (SpawnRadius <= 0.f) return BaseTM;

	FVector Loc = BaseTM.GetLocation();

	// 원형 랜덤 오프셋 (2D)
	const FVector2D Offset2D = FMath::RandPointInCircle(SpawnRadius);
	Loc.X += Offset2D.X;
	Loc.Y += Offset2D.Y;

	FTransform Out = BaseTM;
	Out.SetLocation(Loc);
	return Out;
}
