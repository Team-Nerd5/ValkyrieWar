#include "GameSystem/Base/BaseUnitSpawner.h"

#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "GameSystem/Instance/World/SpawnUpgradeSubsystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/UnitModule.h"

ABaseUnitSpawner::ABaseUnitSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

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

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnSpawnUnitDataReady.AddUniqueDynamic(
			this, &ABaseUnitSpawner::RequestUnitDataToSpawn);

		if (Team == ETeamType::Ally)
		{
			WorldEventSystem->Battle.OnSpawnLevelUpgraded.AddUniqueDynamic(
				this, &ABaseUnitSpawner::HandleSpawnLevelUpgraded);
		}
	}
}

void ABaseUnitSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSpawning();
	GetWorldTimerManager().ClearTimer(CleanupHandle);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnSpawnUnitDataReady.RemoveDynamic(
			this, &ABaseUnitSpawner::RequestUnitDataToSpawn);

		WorldEventSystem->Battle.OnSpawnLevelUpgraded.RemoveDynamic(
			this, &ABaseUnitSpawner::HandleSpawnLevelUpgraded);
	}

	Super::EndPlay(EndPlayReason);
}

void ABaseUnitSpawner::SetSpawnEntries(const TArray<FSpawnUnitEntry>& InEntries)
{
	SpawnEntries = InEntries;
	InitedPools.Reset();
}

void ABaseUnitSpawner::ClearSpawnEntries()
{
	StopSpawning();
	SpawnEntries.Reset();
	InitedPools.Reset();
}

void ABaseUnitSpawner::UpdateEntrySpawnCount(int32 InFamilyId, int32 InSpawnCount)
{
	const int32 Index = FindEntryIndexByFamilyId(InFamilyId);
	if (!SpawnEntries.IsValidIndex(Index)) return;

	SpawnEntries[Index].SpawnCount = FMath::Max(0, InSpawnCount);
}

void ABaseUnitSpawner::StartSpawning()
{
	UE_LOG(LogTemp, Warning, TEXT("[Spawner] StartSpawning called. Team=%d SpawnerId=%d Entries=%d"),
		(int32)Team, SpawnerId, SpawnEntries.Num());

	GetWorldTimerManager().ClearTimer(SpawnTickHandle);

	if (!TryInitPools())
	{
		UE_LOG(LogTemp, Error, TEXT("[Spawner] TryInitPools failed."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Spawner] StartSpawning success. Interval=%.2f"), SpawnInterval);

	BP_OnSpawningStarted();

	const float Interval = FMath::Max(0.01f, SpawnInterval);
	GetWorldTimerManager().SetTimer(
		SpawnTickHandle,
		this,
		&ABaseUnitSpawner::HandleSpawnTick,
		Interval,
		true
	);
}

void ABaseUnitSpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTickHandle);

	if (UObjectPoolSubsystem* Pool = GetPool())
	{
		for (TWeakObjectPtr<AUnitCharacter>& W : SpawnedUnits)
		{
			if (AUnitCharacter* Unit = W.Get())
			{
				if (Unit->IsInPool()) continue;

				if (Unit->GetMyPoolType() != EPoolTypes::None)
				{
					Pool->Release<AUnitCharacter>(Unit->GetMyPoolType(), Unit);
				}
				else
				{
					Unit->Destroy();
				}
			}
		}
	}

	SpawnedUnits.Reset();
}

void ABaseUnitSpawner::NotifyUnitReleased(AUnitCharacter* Unit)
{
	UnregisterSpawnedUnit(Unit);
}

bool ABaseUnitSpawner::TryInitPools()
{
	for (const FSpawnUnitEntry& Entry : SpawnEntries)
	{
		if (!TryInitPoolForEntry(Entry))
		{
			return false;
		}
	}
	return true;
}

bool ABaseUnitSpawner::TryInitPoolForEntry(const FSpawnUnitEntry& Entry)
{
	if (Entry.PoolType == EPoolTypes::None || !Entry.UnitClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[UnitSpawner] Invalid entry. FamilyId=%d UnitDataId=%d"),
			Entry.FamilyId, Entry.UnitDataId);
		return false;
	}

	UObjectPoolSubsystem* Pool = GetPool();
	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("[UnitSpawner] Pool subsystem not found"));
		return false;
	}

	if (const TSubclassOf<AUnitCharacter>* FoundClass = InitedPools.Find(Entry.PoolType))
	{
		if (*FoundClass != Entry.UnitClass)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[UnitSpawner] PoolType conflict. PoolType=%d already initialized with different class."),
				static_cast<int32>(Entry.PoolType));
			return false;
		}
		return true;
	}

	Pool->InitPool<AUnitCharacter>(Entry.PoolType, Entry.UnitClass, Entry.ReserveSize);
	InitedPools.Add(Entry.PoolType, Entry.UnitClass);
	return true;
}

void ABaseUnitSpawner::HandleSpawnTick()
{
	if (SpawnEntries.IsEmpty()) return;
	if (!TryInitPools()) return;

	CompactSpawnedUnits();

	int32 Budget = FMath::Max(0, MaxSpawnPerTick);
	if (Budget <= 0) return;

	for (const FSpawnUnitEntry& Entry : SpawnEntries)
	{
		if (Budget <= 0) break;
		if (Entry.SpawnCount <= 0) continue;

		const int32 SpawnNum = FMath::Min(Entry.SpawnCount, Budget);

		for (int32 i = 0; i < SpawnNum; ++i)
		{
			SpawnOne(Entry);
		}

		Budget -= SpawnNum;
	}
}

void ABaseUnitSpawner::SpawnOne(const FSpawnUnitEntry& Entry)
{
	UObjectPoolSubsystem* Pool = GetPool();
	if (!Pool) return;

	const FTransform SpawnTM = MakeSpawnTransform();

	AUnitCharacter* Unit = Pool->Get<AUnitCharacter>(
		Entry.PoolType,
		SpawnTM.GetLocation(),
		SpawnTM.Rotator()
	);

	if (!Unit) return;

	Unit->SetOwnerSpawner(this);
	Unit->SetPoolType(Entry.PoolType);

	if (UUnitData* Data = ResolveUnitDataObject(Entry.UnitDataId))
	{
		if (Entry.bUseComputedStat)
		{
			Unit->SetComputedEnemyData(Data, Entry.ComputedStat);
		}
		else
		{
			Unit->SetData(Data);
		}
	}

	RegisterSpawnedUnit(Unit);
}

UUnitData* ABaseUnitSpawner::ResolveUnitDataObject(int32 DataId) const
{
	if (DataId <= 0) return nullptr;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDataManager* DataManager = GI->GetSubsystem<UDataManager>())
		{
			if (UUnitModule* UnitModule = DataManager->GetUnitModule())
			{
				return UnitModule->GetUnitDataById(DataId);
			}
		}
	}
	return nullptr;
}

void ABaseUnitSpawner::HandleCleanupTick()
{
	CompactSpawnedUnits();
}

void ABaseUnitSpawner::HandleUnitDestroyed(AActor* DestroyedActor)
{
	UnregisterSpawnedUnit(DestroyedActor);
}

UObjectPoolSubsystem* ABaseUnitSpawner::GetPool() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UObjectPoolSubsystem>();
	}
	return nullptr;
}

void ABaseUnitSpawner::RequestUnitDataToSpawn()
{
	if (UWorld* World = GetWorld())
	{
		if (USpawnUpgradeSubsystem* SpawnUpgradeSubsystem = World->GetSubsystem<USpawnUpgradeSubsystem>())
		{
			SpawnUpgradeSubsystem->SetupSpawnerEntries(this);

			if (SpawnEntries.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] No spawn entries configured."));
				return;
			}

			if (bAutoStart)
			{
				StartSpawning();
			}
		}
	}
}

void ABaseUnitSpawner::HandleSpawnLevelUpgraded(int32 InFamilyId, int32 OldLevel, int32 NewLevel)
{
	UE_LOG(LogTemp, Log, TEXT("HandleSpawnLevelUpgraded : %d"), NewLevel);
	const int32 Index = FindEntryIndexByFamilyId(InFamilyId);
	if (!SpawnEntries.IsValidIndex(Index)) return;

	// 규칙: SpawnCount == Level
	SpawnEntries[Index].SpawnCount = FMath::Max(0, NewLevel);
}

FTransform ABaseUnitSpawner::MakeSpawnTransform() const
{
	const FTransform BaseTM = SpawnPointActor ? SpawnPointActor->GetActorTransform() : GetActorTransform();

	if (SpawnHalfExtent.X <= 0.f && SpawnHalfExtent.Y <= 0.f)
	{
		return BaseTM;
	}

	const float DX = FMath::FRandRange(-SpawnHalfExtent.X, SpawnHalfExtent.X);
	const float DY = FMath::FRandRange(-SpawnHalfExtent.Y, SpawnHalfExtent.Y);

	const FVector Forward = BaseTM.GetRotation().GetForwardVector();
	const FVector Right = BaseTM.GetRotation().GetRightVector();

	FVector Loc = BaseTM.GetLocation();
	Loc += Forward * DX;
	Loc += Right * DY;

	FTransform Out = BaseTM;
	Out.SetLocation(Loc);
	return Out;
}

void ABaseUnitSpawner::RegisterSpawnedUnit(AUnitCharacter* Unit)
{
	if (!Unit) return;

	for (const TWeakObjectPtr<AUnitCharacter>& W : SpawnedUnits)
	{
		if (W.Get() == Unit) return;
	}

	SpawnedUnits.Add(Unit);

	Unit->OnDestroyed.RemoveAll(this);
	Unit->OnDestroyed.AddDynamic(this, &ABaseUnitSpawner::HandleUnitDestroyed);
}

void ABaseUnitSpawner::UnregisterSpawnedUnit(AActor* UnitActor)
{
	if (!UnitActor) return;

	for (int32 i = SpawnedUnits.Num() - 1; i >= 0; --i)
	{
		AUnitCharacter* Unit = SpawnedUnits[i].Get();
		if (!Unit || Unit == UnitActor)
		{
			SpawnedUnits.RemoveAtSwap(i);
		}
	}
}

void ABaseUnitSpawner::CompactSpawnedUnits()
{
	for (int32 i = SpawnedUnits.Num() - 1; i >= 0; --i)
	{
		AUnitCharacter* Unit = SpawnedUnits[i].Get();
		if (!Unit || !IsValid(Unit) || Unit->IsInPool())
		{
			SpawnedUnits.RemoveAtSwap(i);
		}
	}
}

int32 ABaseUnitSpawner::FindEntryIndexByFamilyId(int32 InFamilyId) const
{
	for (int32 i = 0; i < SpawnEntries.Num(); ++i)
	{
		if (SpawnEntries[i].FamilyId == InFamilyId)
		{
			return i;
		}
	}
	return INDEX_NONE;
}
