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

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		if (!AllyUnitIdList.IsEmpty())
		{
			WorldEventSystem->Battle.OnAllyUnitListReady.Broadcast(AllyUnitIdList);
		}
		WorldEventSystem->Battle.OnSpawnUnitDataReady.Broadcast();
	}
}

void USpawnUpgradeSubsystem::RequestDataId(ABaseUnitSpawner* InSpawner)
{
	if (!InSpawner) return;

	const TArray<int32>& TargetList =
		(InSpawner->GetTeam() == ETeamType::Ally) ? AllyUnitIdList : EnemyUnitIdList;

	const int32 SpawnerId = InSpawner->GetSpawnerId();
	if (!TargetList.IsValidIndex(SpawnerId))
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnUpgradeSubsystem] Invalid SpawnerId=%d, ListNum=%d"),
			SpawnerId, TargetList.Num());
		return;
	}

	InSpawner->SetSpawnUnitData(TargetList[SpawnerId]);
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

int32 USpawnUpgradeSubsystem::GetSpawnLevel(int32 FamilyId) const
{
	if (FamilyId <= 0) return 0;

	if (const int32* Found = SpawnLevels.Find(FamilyId))
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
		// 마나 변경으로 affordable이 바뀔 수 있으므로 전체 갱신
		SyncAll();
	}
}

void USpawnUpgradeSubsystem::AddMana(int32 InDeltaMana)
{
	SetCurrentMana(CurrentMana + InDeltaMana, true);
}

void USpawnUpgradeSubsystem::SetCostRule(int32 FamilyId, const FUpgradeCostRule& Rule, bool bBroadcast)
{
	if (FamilyId <= 0) return;

	CostRules.Add(FamilyId, Rule);
	InitFamilyIfNeeded(FamilyId, DefaultSpawnLevel);

	if (bBroadcast)
	{
		BroadcastState(FamilyId);
	}
}

void USpawnUpgradeSubsystem::InitFamilyIfNeeded(int32 InFamilyId, int32 InDefaultLevel)
{
	if (InFamilyId <= 0) return;
	if (SpawnLevels.Contains(InFamilyId)) return;

	const int32 Level = FMath::Max(0, InDefaultLevel);
	SpawnLevels.Add(InFamilyId, Level);
}

int32 USpawnUpgradeSubsystem::CalcCost(int32 InFamilyId, int32 InCurrentLevel) const
{
	if (InCurrentLevel < 0) return 0;

	const FUpgradeCostRule* Rule = CostRules.Find(InFamilyId);
	if (!Rule)
	{
		// 룰이 없으면 프로토 기본값
		return FMath::Max(30, 30 + InCurrentLevel * 10);
	}

	return Rule->BaseCost + InCurrentLevel * Rule->CostStep;
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

	// 업그레이드 비용(현재 레벨 기준)
	const int32 Cost = CalcCost(InFamilyId, Level);

	// 비용 부족이면 UI만 갱신
	if (!SpendMana(Cost))
	{
		BroadcastState(InFamilyId);
		return;
	}

	// 확정 업그레이드
	Level += 1;
	const int32 NewLevel = Level;

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		// 1) 스포너에 승인 통지 (실제 스폰/레벨업은 스포너가 담당)
		WorldEventSystem->Battle.OnSpawnLevelUpgraded.Broadcast(InFamilyId, OldLevel, NewLevel);
	}

	// 2) UI 상태 갱신
	// 마나가 변했으므로 다른 카드 affordability도 같이 변할 수 있어 전체 Sync
	SyncAll();
}

void USpawnUpgradeSubsystem::EnsureFamily(int32 FamilyId, int32 DefaultLevel, bool bBroadcast)
{
	InitFamilyIfNeeded(FamilyId, DefaultLevel);

	if (bBroadcast)
	{
		BroadcastState(FamilyId);
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
	// 룰이 등록되었지만 아직 레벨 초기화가 안된 family가 있을 수 있으니 CostRules도 훑어줌
	for (const TPair<int32, FUpgradeCostRule>& RulePair : CostRules)
	{
		InitFamilyIfNeeded(RulePair.Key, DefaultSpawnLevel);
	}

	for (const TPair<int32, int32>& Pair : SpawnLevels)
	{
		BroadcastState(Pair.Key);
	}
}
