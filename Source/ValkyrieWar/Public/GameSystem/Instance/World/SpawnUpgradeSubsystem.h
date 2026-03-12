#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameSystem/Base/BaseUnitSpawner.h"
#include "SpawnUpgradeSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FUpgradeCostRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BaseCost = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CostStep = 10;
};

class ABaseUnitSpawner;

UCLASS()
class VALKYRIEWAR_API USpawnUpgradeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void InitUnitDataIds();

	// 팀별 집합 스포너 엔트리 구성
	void SetupSpawnerEntries(ABaseUnitSpawner* InSpawner);

	void BindUpgradeDelegates();
	void UnbindUpgradeDelegates();

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	int32 GetSpawnLevel(int32 InFamilyId) const;

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	int32 GetCurrentMana() const { return CurrentMana; }

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void SetCurrentMana(int32 InMana, bool bBroadcastAll = true);

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void AddMana(int32 InDeltaMana);

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void SetCostRule(int32 InFamilyId, const FUpgradeCostRule& Rule, bool bBroadcast = true);

	UFUNCTION()
	void HandleUpgradeClicked(int32 InFamilyId);

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void EnsureFamily(int32 InFamilyId, int32 InDefaultLevel = 0, bool bBroadcast = true);

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void SyncAll();

private:
	void BuildEntriesForTeam(ETeamType InTeam, TArray<FSpawnUnitEntry>& OutEntries);
	int32 ResolveInitialSpawnCount(ETeamType InTeam, int32 InFamilyId) const;

	void InitFamilyIfNeeded(int32 InFamilyId, int32 InDefaultLevel = 0);
	int32 CalcCost(int32 FamilyId, int32 CurrentLevel) const;
	bool CanAfford(int32 Cost) const { return CurrentMana >= Cost; }
	bool SpendMana(int32 Cost);
	void BroadcastState(int32 FamilyId);

private:
	UPROPERTY()
	TArray<int32> UnitDataIdList;

	UPROPERTY()
	TArray<int32> AllyUnitIdList;

	UPROPERTY()
	TArray<int32> EnemyUnitIdList;

	UPROPERTY()
	TMap<int32, int32> SpawnLevels;

	UPROPERTY()
	TMap<int32, FUpgradeCostRule> CostRules;

	UPROPERTY()
	int32 CurrentMana = 0;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnUpgrade")
	int32 DefaultSpawnLevel = 0;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnUpgrade")
	int32 DefaultEnemySpawnCount = 2;
};
