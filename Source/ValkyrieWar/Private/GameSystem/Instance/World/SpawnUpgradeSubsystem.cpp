#include "GameSystem/Instance/World/SpawnUpgradeSubsystem.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Base/BaseUnitSpawner.h"
#include "Data/Module/UnitModule.h"

void USpawnUpgradeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USpawnUpgradeSubsystem::Deinitialize()
{
	SpawnLevels.Reset();
	CostRules.Reset();
	CurrentMana = 0;
	UnitDataIdList.Reset();
	AllyUnitIdList.Reset();
	EnemyUnitIdList.Reset();

	Super::Deinitialize();
}

void USpawnUpgradeSubsystem::InitUnitDataIds()
{
	UnitDataIdList.Reset();
	AllyUnitIdList.Reset();
	EnemyUnitIdList.Reset();

	UWorld* World = GetWorld();
	if (!World) return;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return;

	UDataManager* DataManager = GI->GetSubsystem<UDataManager>();
	if (!DataManager) return;

	UUnitModule* UnitModule = DataManager->GetUnitModule();
	if (!UnitModule) return;

	UnitModule->GetOwnedUnitIds(UnitDataIdList);
	if (UnitDataIdList.IsEmpty()) return;

	for (int32 SingleId : UnitDataIdList)
	{
		if (UnitModule->GetUnitTeam(SingleId) == ETeamType::Ally)
		{
			AllyUnitIdList.AddUnique(SingleId);
		}
		else
		{
			EnemyUnitIdList.AddUnique(SingleId);
		}
	}

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

	UWorld* World = GetWorld();
	if (!World) return;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return;

	UDataManager* DataManager = GI->GetSubsystem<UDataManager>();
	if (!DataManager) return;

	UUnitModule* UnitModule = DataManager->GetUnitModule();
	if (!UnitModule) return;

	const TArray<int32>& SourceIds = (InTeam == ETeamType::Ally) ? AllyUnitIdList : EnemyUnitIdList;

	for (int32 UnitDataId : SourceIds)
	{
		FSpawnUnitEntry Entry;
		Entry.FamilyId = UnitDataId; // 현재는 동일 키 사용
		Entry.UnitDataId = UnitDataId;
		Entry.UnitClass = UnitModule->GetSpawnUnitClass(UnitDataId);
		Entry.PoolType = UnitModule->GetUnitPoolType(UnitDataId);
		Entry.ReserveSize = 30;
		Entry.SpawnCount = ResolveInitialSpawnCount(InTeam, Entry.FamilyId);

		if (Entry.UnitClass && Entry.PoolType != EPoolTypes::None)
		{
			OutEntries.Add(Entry);
		}
	}
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
	if (InFamilyId <= 0) return 0;

	if (const int32* Found = SpawnLevels.Find(InFamilyId))
	{
		return *Found;
	}

	return 0;
}

void USpawnUpgradeSubsystem::SetCurrentMana(int32 InMana, bool bBroadcastAll)
{
	CurrentMana = FMath::Max(0, InMana);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnCurrentManaChanged.Broadcast(CurrentMana);
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
