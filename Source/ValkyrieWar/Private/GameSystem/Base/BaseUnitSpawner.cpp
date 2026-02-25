// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseUnitSpawner.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "GameSystem/Instance/Game/DataManager.h"

ABaseUnitSpawner::ABaseUnitSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	UObjectPoolSubsystem* Pool = GetPool();
	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("[UnitSpawner] Pool subsystem not found"));
		return;
	}

	// 1) 풀 초기화(Reserve)
	for (const FPoolEntry& Entry : PoolEntries)
	{
		if (Entry.PoolType == EPoolTypes::None || !Entry.UnitClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] Invalid PoolEntry"));
			continue;
		}

		Pool->InitPool<AUnitCharacter>(Entry.PoolType, Entry.UnitClass, Entry.ReserveSize);
	}

	// 2) 안전장치 Cleanup 타이머
	if (CleanupInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			CleanupHandle,
			this,
			&ABaseUnitSpawner::HandleCleanupTick,
			CleanupInterval,
			true
		);
	}

	// 3) 자동 시작
	if (bAutoStart)
	{
		StartWaves();
	}
}

void ABaseUnitSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopWaves();
	GetWorldTimerManager().ClearTimer(CleanupHandle);

	Super::EndPlay(EndPlayReason);
}

void ABaseUnitSpawner::StartWaves()
{
	StopWaves();

	if (Waves.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] No Waves configured"));
		return;
	}

	CurrentWaveIndex = 0;
	StartWaveInternal(CurrentWaveIndex);
}

void ABaseUnitSpawner::StopWaves()
{
	GetWorldTimerManager().ClearTimer(WaveStartHandle);
	GetWorldTimerManager().ClearTimer(SpawnTickHandle);
	GetWorldTimerManager().ClearTimer(WaveEndHandle);

	CurrentWaveIndex = INDEX_NONE;
	SpawnedThisWave = 0;

	// 살아있는 애들 정리(원하면 옵션으로)
	if (UObjectPoolSubsystem* Pool = GetPool())
	{
		for (TWeakObjectPtr<AUnitCharacter>& W : AliveUnits)
		{
			if (AUnitCharacter* U = W.Get())
			{
				// 이미 풀로 돌아간 애면 스킵
				if (U->IsInPool()) continue;

				// 안전하게 풀로 반환(유닛 OnRelease에서 BD 해제 + Notify)
				if (U->GetMyPoolType() != EPoolTypes::None)
				{
					Pool->Release<AUnitCharacter>(U->GetMyPoolType(), U);
				}
				else
				{
					U->Destroy();
				}
			}
		}
	}

	AliveUnits.Reset();
}

void ABaseUnitSpawner::NotifyUnitReleased(AUnitCharacter* Unit)
{
	UnregisterAlive(Unit);
}

void ABaseUnitSpawner::StartWaveInternal(int32 WaveIndex)
{
	if (!Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] Invalid wave index: %d"), WaveIndex);
		StopWaves();
		return;
	}

	SpawnedThisWave = 0;

	const FWaveConfig& Wave = Waves[WaveIndex];
	UE_LOG(LogTemp, Log, TEXT("[UnitSpawner] StartWave %d StartDelay=%.2f Interval=%.2f Total=%d MaxAlive=%d"),
		WaveIndex, Wave.StartDelay, Wave.SpawnInterval, Wave.TotalToSpawn, Wave.MaxAlive);

	const float StartDelay = FMath::Max(0.f, Wave.StartDelay);

	GetWorldTimerManager().ClearTimer(WaveStartHandle);
	if (StartDelay <= 0.f)
	{
		HandleWaveStart();
	}
	else
	{
		GetWorldTimerManager().SetTimer(
			WaveStartHandle,
			this,
			&ABaseUnitSpawner::HandleWaveStart,
			StartDelay,
			false
		);
	}
}

void ABaseUnitSpawner::HandleWaveStart()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		StopWaves();
		return;
	}

	BP_OnWaveStarted(CurrentWaveIndex);

	const FWaveConfig& Wave = Waves[CurrentWaveIndex];
	const float Interval = FMath::Max(0.05f, Wave.SpawnInterval);

	UE_LOG(LogTemp, Log, TEXT("[UnitSpawner] Wave %d spawning started. Interval=%.2f"), CurrentWaveIndex, Interval);

	GetWorldTimerManager().ClearTimer(SpawnTickHandle);
	GetWorldTimerManager().SetTimer(
		SpawnTickHandle,
		this,
		&ABaseUnitSpawner::HandleSpawnTick,
		Interval,
		true
	);
}

void ABaseUnitSpawner::HandleSpawnTick()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		StopWaves();
		return;
	}

	CompactAliveUnits();

	const FWaveConfig& Wave = Waves[CurrentWaveIndex];

	int32 Count = FMath::Min(MaxSpawnCount, Wave.SpawnCount);

	for (int32 i = 0; i < Count; i++)
	{
		// 종료 조건을 MaxAlive보다 먼저 검사
		if (Wave.TotalToSpawn > 0 && SpawnedThisWave >= Wave.TotalToSpawn)
		{
			EndWaveInternal();
			return;
		}

		// 스포너 단위 MaxAlive
		if (AliveUnits.Num() >= Wave.MaxAlive)
		{
			return;
		}

		const EPoolTypes PickType = PickWeightedPoolType(Wave);
		if (PickType == EPoolTypes::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] Options invalid for wave %d"), CurrentWaveIndex);
			return;
		}

		const FPoolEntry* Entry = FindPoolEntry(PickType);
		if (!Entry || !Entry->UnitClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[UnitSpawner] No PoolEntry for PoolType=%d"), (int32)PickType);
			return;
		}

		UObjectPoolSubsystem* Pool = GetPool();
		if (!Pool)
		{
			UE_LOG(LogTemp, Error, TEXT("[UnitSpawner] Pool subsystem missing"));
			return;
		}

		const FTransform SpawnTM = MakeSpawnTransform();

		AUnitCharacter* Unit = Pool->Get<AUnitCharacter>(
			PickType,
			Entry->UnitClass,
			SpawnTM.GetLocation(),
			SpawnTM.Rotator()
		);

		if (!Unit) return;

		// 유닛이 OnRelease에서 스포너에게 Alive 감소를 Notify할 수 있도록 소유 스포너 지정
		Unit->SetOwnerSpawner(this);
		Unit->SetPoolType(PickType);

		UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();
		if (DataManager && DataManager->GetUnitModule())
		{
			if (UUnitData* InData = DataManager->GetUnitModule()->GetUnitDataById(Entry->UnitDataId))
			{
				Unit->SetData(InData);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] UnitData not found. PoolType=%d UnitDataId=%d"),
					(int32)PickType, Entry->UnitDataId);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] UnitModule is null (cannot SetData)."));
		}

		RegisterAlive(Unit);
		SpawnedThisWave++;

		BP_OnUnitSpawned(Unit, CurrentWaveIndex, PickType);
	}
}

void ABaseUnitSpawner::EndWaveInternal()
{
	GetWorldTimerManager().ClearTimer(SpawnTickHandle);

	const int32 Finished = CurrentWaveIndex;
	BP_OnWaveFinished(Finished);

	const float EndDelay = FMath::Max(0.f, Waves[Finished].EndDelay);
	UE_LOG(LogTemp, Log, TEXT("[UnitSpawner] Wave %d finished. EndDelay=%.2f"), Finished, EndDelay);

	GetWorldTimerManager().ClearTimer(WaveEndHandle);
	if (EndDelay <= 0.f)
	{
		HandleWaveEnd();
	}
	else
	{
		GetWorldTimerManager().SetTimer(
			WaveEndHandle,
			this,
			&ABaseUnitSpawner::HandleWaveEnd,
			EndDelay,
			false
		);
	}
}

void ABaseUnitSpawner::HandleWaveEnd()
{
	CurrentWaveIndex++;

	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		if (bLoopWaves && Waves.Num() > 0)
		{
			CurrentWaveIndex = 0;
			StartWaveInternal(CurrentWaveIndex);
		}
		else
		{
			StopWaves();
		}
		return;
	}

	StartWaveInternal(CurrentWaveIndex);
}

void ABaseUnitSpawner::HandleCleanupTick()
{
	CompactAliveUnits();
}

void ABaseUnitSpawner::HandleUnitDestroyed(AActor* DestroyedActor)
{
	UnregisterAlive(DestroyedActor);
}

UObjectPoolSubsystem* ABaseUnitSpawner::GetPool() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UObjectPoolSubsystem>();
	}
	return nullptr;
}

const FPoolEntry* ABaseUnitSpawner::FindPoolEntry(EPoolTypes PoolType) const
{
	for (const FPoolEntry& E : PoolEntries)
	{
		if (E.PoolType == PoolType) return &E;
	}
	return nullptr;
}

EPoolTypes ABaseUnitSpawner::PickWeightedPoolType(const FWaveConfig& Wave) const
{
	int32 Total = 0;
	for (const FWaveOption& Opt : Wave.Options)
	{
		if (Opt.PoolType != EPoolTypes::None && Opt.Weight > 0)
		{
			Total += Opt.Weight;
		}
	}
	if (Total <= 0) return EPoolTypes::None;

	int32 Pick = FMath::RandRange(1, Total);
	for (const FWaveOption& Opt : Wave.Options)
	{
		if (Opt.PoolType == EPoolTypes::None || Opt.Weight <= 0) continue;
		Pick -= Opt.Weight;
		if (Pick <= 0) return Opt.PoolType;
	}

	// 안전 fallback
	for (const FWaveOption& Opt : Wave.Options)
	{
		if (Opt.PoolType != EPoolTypes::None) return Opt.PoolType;
	}
	return EPoolTypes::None;
}

FTransform ABaseUnitSpawner::MakeSpawnTransform() const
{
	const FTransform BaseTM = SpawnPointActor ? SpawnPointActor->GetActorTransform() : GetActorTransform();
	if (SpawnRadius <= 0.f) return BaseTM;

	FVector Loc = BaseTM.GetLocation();
	const FVector2D Offset2D = FMath::RandPointInCircle(SpawnRadius);
	Loc.X += Offset2D.X;
	Loc.Y += Offset2D.Y;

	FTransform Out = BaseTM;
	Out.SetLocation(Loc);
	return Out;
}

void ABaseUnitSpawner::RegisterAlive(AUnitCharacter* Unit)
{
	if (!Unit) return;

	// 중복 방지
	for (const TWeakObjectPtr<AUnitCharacter>& W : AliveUnits)
	{
		if (W.Get() == Unit)
		{
			return;
		}
	}

	AliveUnits.Add(Unit);

	// Destroy() fallback 대비용
	Unit->OnDestroyed.RemoveAll(this);
	Unit->OnDestroyed.AddDynamic(this, &ABaseUnitSpawner::HandleUnitDestroyed);
}

void ABaseUnitSpawner::UnregisterAlive(AActor* UnitActor)
{
	if (!UnitActor) return;

	for (int32 i = AliveUnits.Num() - 1; i >= 0; --i)
	{
		AUnitCharacter* U = AliveUnits[i].Get();
		if (!U || U == UnitActor)
		{
			AliveUnits.RemoveAtSwap(i);
		}
	}
}

void ABaseUnitSpawner::CompactAliveUnits()
{
	for (int32 i = AliveUnits.Num() - 1; i >= 0; --i)
	{
		AUnitCharacter* Unit = AliveUnits[i].Get();

		// 1) 이미 GC/Destroy
		if (!Unit || !IsValid(Unit))
		{
			AliveUnits.RemoveAtSwap(i);
			continue;
		}

		// 2) Notify 누락 대비: 풀로 돌아간 상태면 Alive에서 제거
		if (Unit->IsInPool())
		{
			AliveUnits.RemoveAtSwap(i);
			continue;
		}
	}
}
