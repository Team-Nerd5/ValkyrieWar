#include "GameSystem/Instance/World/SpawnUpgradeSubsystem.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void USpawnUpgradeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize"));
	Super::Initialize(Collection);
}

void USpawnUpgradeSubsystem::Deinitialize()
{
	SpawnLevels.Reset();
	CostRules.Reset();
	CurrentMana = 0;

	Super::Deinitialize();
}

void USpawnUpgradeSubsystem::BindUpgradeDelegates()
{
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Bind] Sub=%p World=%s ES=%p"),
			this, *GetWorld()->GetName(), WorldEventSystem);

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
		SyncAll(); // 마나로 affordable이 변하므로 전체 갱신
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
		return FMath::Max(30, 30 + (InCurrentLevel) * 10);
	}

	return Rule->BaseCost + (InCurrentLevel) * Rule->CostStep;
}

bool USpawnUpgradeSubsystem::IsMax(int32 InFamilyId, int32 InCurrentLevel) const
{
	const FUpgradeCostRule* Rule = CostRules.Find(InFamilyId);
	if (!Rule) return false;
	return InCurrentLevel >= Rule->MaxLevel;
}

bool USpawnUpgradeSubsystem::SpendMana(int32 Cost)
{
	if (Cost <= 0) return true; // 무료 업그레이드(테스트용)
	if (!CanAfford(Cost)) return false;

	CurrentMana -= Cost;
	CurrentMana = FMath::Max(0, CurrentMana);
	return true;
}

void USpawnUpgradeSubsystem::HandleUpgradeClicked(int32 InFamilyId)
{
	UE_LOG(LogTemp, Warning, TEXT("USpawnUpgradeSubsystem::HandleUpgradeClicked! : %d"), InFamilyId);

	if (InFamilyId <= 0) return;

	InitFamilyIfNeeded(InFamilyId, 1);

	int32& Level = SpawnLevels.FindOrAdd(InFamilyId);
	const int32 OldLevel = Level;

	// MAX면 UI만 갱신
	if (IsMax(InFamilyId, Level))
	{
		UE_LOG(LogTemp, Warning, TEXT("USpawnUpgradeSubsystem::IsMax!"));

		BroadcastState(InFamilyId);
		return;
	}

	// 업그레이드 비용(현재 레벨 기준)
	const int32 Cost = CalcCost(InFamilyId, Level);

	// 비용 부족이면 UI만 갱신
	if (!SpendMana(Cost))
	{
		BroadcastState(InFamilyId);
		UE_LOG(LogTemp, Warning, TEXT("비용부족!"));
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

	InitFamilyIfNeeded(FamilyId, 1);

	const int32 Level = GetSpawnLevel(FamilyId);
	const bool bMax = IsMax(FamilyId, Level);

	const int32 Cost = bMax ? 0 : CalcCost(FamilyId, Level);
	const bool bAffordable = (!bMax) && CanAfford(Cost);

	WorldEventSystem->Battle.OnUpgradeStateChanged.Broadcast(
		FamilyId, Level, Cost, bAffordable, bMax
	);
}

void USpawnUpgradeSubsystem::SyncAll()
{
	// 룰이 등록되었지만 아직 레벨 초기화가 안된 가족이 있을 수 있으니, CostRules도 같이 훑어줌(안정성)
	for (const TPair<int32, FUpgradeCostRule>& RulePair : CostRules)
	{
		InitFamilyIfNeeded(RulePair.Key, 1);
	}

	for (const TPair<int32, int32>& Pair : SpawnLevels)
	{
		BroadcastState(Pair.Key);
	}
}
