#include "Test/SDCH/TestActors/TestUnitSpawner.h"
#include "Math/UnrealMathUtility.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "Components/PrimitiveComponent.h"

ATestUnitSpawner::ATestUnitSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATestUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	UObjectPoolSubsystem* Pool = GetPool();
	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("[UnitSpawner] Pool subsystem not found"));
		return;
	}

	static TSet<EPoolTypes> InitializedTypes;

	// 1) 풀 초기화(Reserve)
	for (const FTestPoolEntry& Entry : PoolEntries)
	{
		if (InitializedTypes.Contains(Entry.PoolType))
		{
			continue; // 이미 초기화됨
		}

		if (Entry.PoolType == EPoolTypes::None || !Entry.UnitClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] Invalid PoolEntry"));
			continue;
		}

		Pool->InitPool<ATestBaseUnit>(Entry.PoolType, Entry.UnitClass, Entry.ReserveSize);

		InitializedTypes.Add(Entry.PoolType);
	}

	// 2) 안전장치 Cleanup 타이머
	if (CleanupInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			CleanupHandle,
			this,
			&ATestUnitSpawner::HandleCleanupTick,
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

void ATestUnitSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopWaves();
	GetWorldTimerManager().ClearTimer(CleanupHandle);

	Super::EndPlay(EndPlayReason);
}

void ATestUnitSpawner::StartWaves()
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

void ATestUnitSpawner::StopWaves()
{
	GetWorldTimerManager().ClearTimer(WaveStartHandle);
	GetWorldTimerManager().ClearTimer(SpawnTickHandle);
	GetWorldTimerManager().ClearTimer(WaveEndHandle);

	CurrentWaveIndex = INDEX_NONE;
	SpawnedThisWave = 0;

	// ✅ 살아있는 애들 정리(원하면 옵션으로)
	if (UObjectPoolSubsystem* Pool = GetPool())
	{
		for (TWeakObjectPtr<ATestBaseUnit>& W : AliveUnits)
		{
			if (ATestBaseUnit* U = W.Get())
			{
				// 이미 풀로 돌아간 애면 스킵
				if (IsReturnedToPoolHeuristic(U)) continue;

				// 안전하게 풀로 반환(유닛 OnRelease에서 BD 해제 + Notify)
				if (U->MyPoolType != EPoolTypes::None)
				{
					Pool->Release<ATestBaseUnit>(U->MyPoolType, U);
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

void ATestUnitSpawner::NotifyUnitReleased(ATestBaseUnit* Unit)
{
	UnregisterAlive(Unit);
}

void ATestUnitSpawner::StartWaveInternal(int32 WaveIndex)
{
	if (!Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] Invalid wave index: %d"), WaveIndex);
		StopWaves();
		return;
	}

	SpawnedThisWave = 0;

	const FTestWaveConfig& Wave = Waves[WaveIndex];
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
			&ATestUnitSpawner::HandleWaveStart,
			StartDelay,
			false
		);
	}
}

void ATestUnitSpawner::HandleWaveStart()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		StopWaves();
		return;
	}

	BP_OnWaveStarted(CurrentWaveIndex);

	const FTestWaveConfig& Wave = Waves[CurrentWaveIndex];
	const float Interval = FMath::Max(0.05f, Wave.SpawnInterval);

	UE_LOG(LogTemp, Log, TEXT("[UnitSpawner] Wave %d spawning started. Interval=%.2f"), CurrentWaveIndex, Interval);

	GetWorldTimerManager().ClearTimer(SpawnTickHandle);
	GetWorldTimerManager().SetTimer(
		SpawnTickHandle,
		this,
		&ATestUnitSpawner::HandleSpawnTick,
		Interval,
		true
	);
}

void ATestUnitSpawner::HandleSpawnTick()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		StopWaves();
		return;
	}

	CompactAliveUnits();

	const FTestWaveConfig& Wave = Waves[CurrentWaveIndex];

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

	const FTestPoolEntry* Entry = FindPoolEntry(PickType);
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

	ATestBaseUnit* Unit = Pool->Get<ATestBaseUnit>(
		PickType,
		Entry->UnitClass,
		SpawnTM.GetLocation(),
		SpawnTM.Rotator()
	);

	if (!Unit) return;

	// 유닛이 OnRelease에서 스포너에게 Alive 감소를 Notify할 수 있도록 소유 스포너 지정
	Unit->SetOwnerSpawner(this);
	Unit->SetPoolType(PickType);

	RegisterAlive(Unit);
	SpawnedThisWave++;

	BP_OnUnitSpawned(Unit, CurrentWaveIndex, PickType);
}

void ATestUnitSpawner::EndWaveInternal()
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
			&ATestUnitSpawner::HandleWaveEnd,
			EndDelay,
			false
		);
	}
}

void ATestUnitSpawner::HandleWaveEnd()
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

void ATestUnitSpawner::HandleCleanupTick()
{
	CompactAliveUnits();
}

void ATestUnitSpawner::HandleUnitDestroyed(AActor* DestroyedActor)
{
	UnregisterAlive(DestroyedActor);
}

UObjectPoolSubsystem* ATestUnitSpawner::GetPool() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UObjectPoolSubsystem>();
	}
	return nullptr;
}

const FTestPoolEntry* ATestUnitSpawner::FindPoolEntry(EPoolTypes PoolType) const
{
	for (const FTestPoolEntry& E : PoolEntries)
	{
		if (E.PoolType == PoolType) return &E;
	}
	return nullptr;
}

EPoolTypes ATestUnitSpawner::PickWeightedPoolType(const FTestWaveConfig& Wave) const
{
	int32 Total = 0;
	for (const FTestWaveOption& Opt : Wave.Options)
	{
		if (Opt.PoolType != EPoolTypes::None && Opt.Weight > 0)
		{
			Total += Opt.Weight;
		}
	}
	if (Total <= 0) return EPoolTypes::None;

	int32 Pick = FMath::RandRange(1, Total);
	for (const FTestWaveOption& Opt : Wave.Options)
	{
		if (Opt.PoolType == EPoolTypes::None || Opt.Weight <= 0) continue;
		Pick -= Opt.Weight;
		if (Pick <= 0) return Opt.PoolType;
	}

	// 안전 fallback
	for (const FTestWaveOption& Opt : Wave.Options)
	{
		if (Opt.PoolType != EPoolTypes::None) return Opt.PoolType;
	}
	return EPoolTypes::None;
}

FTransform ATestUnitSpawner::MakeSpawnTransform() const
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

void ATestUnitSpawner::RegisterAlive(ATestBaseUnit* Unit)
{
	if (!Unit) return;

	// 중복 방지
	for (const TWeakObjectPtr<ATestBaseUnit>& W : AliveUnits)
	{
		if (W.Get() == Unit)
		{
			return;
		}
	}

	AliveUnits.Add(Unit);

	Unit->OnDestroyed.RemoveAll(this);
	Unit->OnDestroyed.AddDynamic(this, &ATestUnitSpawner::HandleUnitDestroyed);
}

void ATestUnitSpawner::UnregisterAlive(AActor* UnitActor)
{
	if (!UnitActor) return;

	for (int32 i = AliveUnits.Num() - 1; i >= 0; --i)
	{
		ATestBaseUnit* U = AliveUnits[i].Get();
		if (!U || U == UnitActor)
		{
			AliveUnits.RemoveAtSwap(i);
		}
	}
}

bool ATestUnitSpawner::IsReturnedToPoolHeuristic(const ATestBaseUnit* Unit) const
{
	// 풀 Release()에서 해주는 세팅과 맞춰 휴리스틱 제거:
	// HiddenInGame=true, Collision=false, Tick=false
	if (!Unit) return true;

	const bool bUnitHidden = Unit->IsHidden();
	const bool bTickDisabled = !Unit->IsActorTickEnabled();

	bool bCollisionDisabled = true;
	if (const UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Unit->GetRootComponent()))
	{
		bCollisionDisabled = (RootPrim->GetCollisionEnabled() == ECollisionEnabled::NoCollision);
	}

	return bUnitHidden && bTickDisabled && bCollisionDisabled;
}

void ATestUnitSpawner::CompactAliveUnits()
{
	for (int32 i = AliveUnits.Num() - 1; i >= 0; --i)
	{
		ATestBaseUnit* Unit = AliveUnits[i].Get();

		// 1) 이미 GC/Destroy
		if (!Unit || !IsValid(Unit))
		{
			AliveUnits.RemoveAtSwap(i);
			continue;
		}

		// 2) ✅ Notify 누락 대비: 풀로 돌아간 상태면 Alive에서 제거
		if (IsReturnedToPoolHeuristic(Unit))
		{
			AliveUnits.RemoveAtSwap(i);
			continue;
		}
	}
}
