#include "GameSystem/Instance/World/SpawnUpgradeSubsystem.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Module/UnitModule.h"
#include "Data/Module/StageModule.h"
#include "Data/Module/StageInfoModule.h"

void USpawnUpgradeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USpawnUpgradeSubsystem::Deinitialize()
{
	SpawnLevels.Reset();
	CostRules.Reset();
	CurrentMana = 0;
	AllyUnitIdList.Reset();
	EnemyUnitIdList.Reset();

	Super::Deinitialize();
}

void USpawnUpgradeSubsystem::InitUnitDataIds()
{
	AllyUnitIdList.Reset();
	EnemyUnitIdList.Reset();

	InitAllyUnitDataIds();
	InitEnemyUnitDataIdsFromSelectedStage();

	// 현재는 FamilyId == UnitDataId
	for (int32 AllyId : AllyUnitIdList)
	{
		InitFamilyIfNeeded(AllyId, DefaultSpawnLevel);
	}

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		if (!AllyUnitIdList.IsEmpty())
		{
			WorldEventSystem->Battle.OnAllyUnitListReady.Broadcast(AllyUnitIdList);
		}

		WorldEventSystem->Battle.OnSpawnUnitDataReady.Broadcast();
	}
}

void USpawnUpgradeSubsystem::InitAllyUnitDataIds()
{
	AllyUnitIdList.Reset();

	UUnitModule* UnitModule = GetUnitModule();
	if (!UnitModule) return;

	TArray<int32> OwnedUnitIds;
	UnitModule->GetOwnedUnitIds(OwnedUnitIds);

	for (int32 UnitDataId : OwnedUnitIds)
	{
		if (UnitModule->GetUnitTeam(UnitDataId) == ETeamType::Ally)
		{
			AllyUnitIdList.AddUnique(UnitDataId);
		}
	}
}

void USpawnUpgradeSubsystem::InitEnemyUnitDataIdsFromSelectedStage()
{
	EnemyUnitIdList.Reset();

	UStageModule* StageModule = GetStageModule();
	UStageInfoModule* StageInfoModule = GetStageInfoModule();
	if (!StageModule || !StageInfoModule)
	{
		return;
	}

	const int32 SelectedStageCode = StageModule->GetSelectedStageCode();
	if (SelectedStageCode <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnUpgradeSubsystem] Invalid selected stage code."));
		return;
	}

	const int32 Chapter = StageModule->GetChapterFromStageCode(SelectedStageCode);
	const int32 StageNum = StageModule->GetStageFromStageCode(SelectedStageCode);

	if (!StageInfoModule->GetEnemyUnitIdsByChapterAndStage(Chapter, StageNum, EnemyUnitIdList))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SpawnUpgradeSubsystem] Failed to get enemy unit ids. Chapter=%d Stage=%d"),
			Chapter, StageNum);
	}
}

void USpawnUpgradeSubsystem::SetupSpawnerEntries(ABaseUnitSpawner* InSpawner)
{
	if (!InSpawner) return;

	TArray<FSpawnUnitEntry> NewEntries;
	BuildEntriesForTeam(InSpawner->GetTeam(), NewEntries);

	InSpawner->SetSpawnEntries(NewEntries);
}

void USpawnUpgradeSubsystem::BuildEntriesForTeam(ETeamType InTeam, TArray<FSpawnUnitEntry>& OutEntries)
{
	OutEntries.Reset();

	UUnitModule* UnitModule = GetUnitModule();
	if (!UnitModule) return;

	if (InTeam == ETeamType::Ally)
	{
		BuildAllyEntries(UnitModule, OutEntries);
	}
	else
	{
		BuildEnemyEntries(UnitModule, OutEntries);
	}
}

void USpawnUpgradeSubsystem::BuildAllyEntries(UUnitModule* InUnitModule, TArray<FSpawnUnitEntry>& OutEntries)
{
	if (!InUnitModule) return;

	for (int32 UnitDataId : AllyUnitIdList)
	{
		FSpawnUnitEntry Entry;
		const int32 SpawnCount = ResolveInitialSpawnCount(ETeamType::Ally, UnitDataId);

		// 현재는 FamilyId == UnitDataId
		if (MakeSpawnEntry(InUnitModule, UnitDataId, UnitDataId, SpawnCount, Entry))
		{
			OutEntries.Add(Entry);
		}
	}
}

void USpawnUpgradeSubsystem::BuildEnemyEntries(UUnitModule* InUnitModule, TArray<FSpawnUnitEntry>& OutEntries)
{
	if (!InUnitModule) return;

	const int32 EnemyLevel = ResolveEnemyLevelFromSelectedStage();

	for (int32 UnitDataId : EnemyUnitIdList)
	{
		FSpawnUnitEntry Entry;
		const int32 SpawnCount = ResolveInitialSpawnCount(ETeamType::Enemy, UnitDataId);

		// 현재는 FamilyId == UnitDataId
		if (MakeSpawnEntry(InUnitModule, UnitDataId, UnitDataId, SpawnCount, Entry))
		{
			Entry.UnitLevel = EnemyLevel;

			// 적 최종 스탯 1회 계산 후 캐시
			FComputedEnemyStat ComputedStat;
			if (InUnitModule->BuildComputedEnemyStat(UnitDataId, EnemyLevel, ComputedStat))
			{
				Entry.bUseComputedStat = true;
				Entry.ComputedStat = ComputedStat;
			}
			else
			{
				Entry.bUseComputedStat = false;
			}

			OutEntries.Add(Entry);
		}
	}
}

bool USpawnUpgradeSubsystem::MakeSpawnEntry(
	UUnitModule* InUnitModule,
	int32 InFamilyId,
	int32 InUnitDataId,
	int32 InSpawnCount,
	FSpawnUnitEntry& OutEntry
) const
{
	if (!InUnitModule) return false;
	if (InUnitDataId <= 0) return false;

	OutEntry.FamilyId = InFamilyId;
	OutEntry.UnitDataId = InUnitDataId;
	OutEntry.UnitClass = InUnitModule->GetSpawnUnitClass(InUnitDataId);
	OutEntry.PoolType = InUnitModule->GetUnitPoolType(InUnitDataId);
	OutEntry.ReserveSize = DefaultReserveSize;
	OutEntry.SpawnCount = FMath::Max(0, InSpawnCount);

	if (!OutEntry.UnitClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SpawnUpgradeSubsystem] UnitClass is null. UnitDataId=%d"),
			InUnitDataId);
		return false;
	}

	if (OutEntry.PoolType == EPoolTypes::None)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SpawnUpgradeSubsystem] PoolType is None. UnitDataId=%d"),
			InUnitDataId);
		return false;
	}

	return true;
}

int32 USpawnUpgradeSubsystem::ResolveEnemyLevelFromSelectedStage() const
{
	UStageModule* StageModule = GetStageModule();
	UStageInfoModule* StageInfoModule = GetStageInfoModule();
	if (!StageModule || !StageInfoModule)
	{
		return DefaultEnemyLevel;
	}

	const int32 SelectedStageCode = StageModule->GetSelectedStageCode();
	if (SelectedStageCode <= 0)
	{
		return DefaultEnemyLevel;
	}

	const int32 Chapter = StageModule->GetChapterFromStageCode(SelectedStageCode);
	const int32 StageNum = StageModule->GetStageFromStageCode(SelectedStageCode);

	int32 OutEnemyLevel = DefaultEnemyLevel;
	if (!StageInfoModule->GetEnemyLevelByChapterAndStage(Chapter, StageNum, OutEnemyLevel))
	{
		return DefaultEnemyLevel;
	}

	return FMath::Max(1, OutEnemyLevel);
}

int32 USpawnUpgradeSubsystem::ResolveInitialSpawnCount(ETeamType InTeam, int32 InFamilyId) const
{
	if (InTeam == ETeamType::Ally)
	{
		return GetSpawnLevel(InFamilyId);
	}

	return DefaultEnemySpawnCount;
}

void USpawnUpgradeSubsystem::BindUpgradeDelegates()
{
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnUpgradeClicked.AddUniqueDynamic(
			this, &USpawnUpgradeSubsystem::HandleUpgradeClicked);

		WorldEventSystem->Battle.OnManaAdd.AddUniqueDynamic(
			this, &USpawnUpgradeSubsystem::AddMana);
	}
}

void USpawnUpgradeSubsystem::UnbindUpgradeDelegates()
{
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnUpgradeClicked.RemoveDynamic(
			this, &USpawnUpgradeSubsystem::HandleUpgradeClicked);

		WorldEventSystem->Battle.OnManaAdd.RemoveDynamic(
			this, &USpawnUpgradeSubsystem::AddMana);
	}
}

int32 USpawnUpgradeSubsystem::GetSpawnLevel(int32 InFamilyId) const
{
	if (InFamilyId <= 0)
	{
		return 0;
	}

	if (const int32* Found = SpawnLevels.Find(InFamilyId))
	{
		return *Found;
	}

	return 0;
}

void USpawnUpgradeSubsystem::SetCurrentMana(int32 InMana, bool bBroadcastAll)
{
	CurrentMana = FMath::Clamp(InMana, 0, MaxMana);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnCurrentManaChanged.Broadcast(CurrentMana, MaxMana);
	}

	if (bBroadcastAll)
	{
		SyncAll();
	}
}

void USpawnUpgradeSubsystem::AddMana(int32 InDeltaMana)
{
	SetCurrentMana(CurrentMana + InDeltaMana, true);
}

void USpawnUpgradeSubsystem::SetCostRule(int32 InFamilyId, const FUpgradeCostRule& Rule, bool bBroadcast)
{
	if (InFamilyId <= 0) return;

	CostRules.Add(InFamilyId, Rule);
	InitFamilyIfNeeded(InFamilyId, DefaultSpawnLevel);

	if (bBroadcast)
	{
		BroadcastState(InFamilyId);
	}
}

void USpawnUpgradeSubsystem::InitFamilyIfNeeded(int32 InFamilyId, int32 InDefaultLevel)
{
	if (InFamilyId <= 0) return;
	if (SpawnLevels.Contains(InFamilyId)) return;

	SpawnLevels.Add(InFamilyId, FMath::Max(0, InDefaultLevel));
}

int32 USpawnUpgradeSubsystem::CalcCost(int32 FamilyId, int32 CurrentLevel) const
{
	if (CurrentLevel < 0) return 0;

	const FUpgradeCostRule* Rule = CostRules.Find(FamilyId);
	if (!Rule)
	{
		return FMath::Max(30, 30 + CurrentLevel * 10);
	}

	return Rule->BaseCost + CurrentLevel * Rule->CostStep;
}

bool USpawnUpgradeSubsystem::SpendMana(int32 Cost)
{
	if (Cost <= 0) return true;
	if (!CanAfford(Cost)) return false;

	SetCurrentMana(CurrentMana - Cost, false);
	return true;
}

void USpawnUpgradeSubsystem::HandleUpgradeClicked(int32 InFamilyId)
{
	if (InFamilyId <= 0) return;

	InitFamilyIfNeeded(InFamilyId, DefaultSpawnLevel);

	int32& Level = SpawnLevels.FindOrAdd(InFamilyId);
	const int32 OldLevel = Level;

	const int32 Cost = CalcCost(InFamilyId, Level);
	if (!SpendMana(Cost))
	{
		BroadcastState(InFamilyId);
		return;
	}

	Level += 1;
	const int32 NewLevel = Level;

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnSpawnLevelUpgraded.Broadcast(InFamilyId, OldLevel, NewLevel);
	}

	SyncAll();
}

void USpawnUpgradeSubsystem::EnsureFamily(int32 InFamilyId, int32 InDefaultLevel, bool bBroadcast)
{
	InitFamilyIfNeeded(InFamilyId, InDefaultLevel);

	if (bBroadcast)
	{
		BroadcastState(InFamilyId);
	}
}

void USpawnUpgradeSubsystem::BroadcastState(int32 FamilyId)
{
	UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	if (!WorldEventSystem) return;

	InitFamilyIfNeeded(FamilyId, DefaultSpawnLevel);

	const int32 Level = GetSpawnLevel(FamilyId);
	const int32 Cost = CalcCost(FamilyId, Level);
	const bool bAffordable = CanAfford(Cost);

	WorldEventSystem->Battle.OnUpgradeStateChanged.Broadcast(
		FamilyId, Level, Cost, bAffordable
	);
}

void USpawnUpgradeSubsystem::SyncAll()
{
	for (const TPair<int32, FUpgradeCostRule>& RulePair : CostRules)
	{
		InitFamilyIfNeeded(RulePair.Key, DefaultSpawnLevel);
	}

	for (const TPair<int32, int32>& Pair : SpawnLevels)
	{
		BroadcastState(Pair.Key);
	}
}

UDataManager* USpawnUpgradeSubsystem::GetDataManager() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return nullptr;

	return GI->GetSubsystem<UDataManager>();
}

UUnitModule* USpawnUpgradeSubsystem::GetUnitModule() const
{
	if (UDataManager* DataManager = GetDataManager())
	{
		return DataManager->GetUnitModule();
	}
	return nullptr;
}

UStageModule* USpawnUpgradeSubsystem::GetStageModule() const
{
	if (UDataManager* DataManager = GetDataManager())
	{
		return DataManager->GetStageModule();
	}
	return nullptr;
}

UStageInfoModule* USpawnUpgradeSubsystem::GetStageInfoModule() const
{
	if (UDataManager* DataManager = GetDataManager())
	{
		return DataManager->GetStageInfoModule();
	}
	return nullptr;
}
