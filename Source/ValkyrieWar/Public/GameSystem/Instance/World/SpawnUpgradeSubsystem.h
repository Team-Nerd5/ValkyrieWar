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
class UDataManager;
class UUnitModule;
class UStageModule;
class UStageInfoModule;

UCLASS()
class VALKYRIEWAR_API USpawnUpgradeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 전투 진입 시 전체 초기화
	void InitUnitDataIds();

	// 팀별 스포너 엔트리 구성
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
	// -------------------------
	// Init Helpers
	// -------------------------
	void InitAllyUnitDataIds();
	void InitEnemyUnitDataIdsFromSelectedStage();

	// -------------------------
	// Build Entry Helpers
	// -------------------------
	void BuildEntriesForTeam(ETeamType InTeam, TArray<FSpawnUnitEntry>& OutEntries);
	void BuildAllyEntries(UUnitModule* InUnitModule, TArray<FSpawnUnitEntry>& OutEntries);
	void BuildEnemyEntries(UUnitModule* InUnitModule, TArray<FSpawnUnitEntry>& OutEntries);

	bool MakeSpawnEntry(
		UUnitModule* InUnitModule,
		int32 InFamilyId,
		int32 InUnitDataId,
		int32 InSpawnCount,
		FSpawnUnitEntry& OutEntry
	) const;

	int32 ResolveInitialSpawnCount(ETeamType InTeam, int32 InFamilyId) const;

	// 나중에 적 레벨 스케일링 붙일 때 사용
	int32 ResolveEnemyLevelFromSelectedStage() const;

	// -------------------------
	// Upgrade / Cost Helpers
	// -------------------------
	void InitFamilyIfNeeded(int32 InFamilyId, int32 InDefaultLevel = 0);
	int32 CalcCost(int32 FamilyId, int32 CurrentLevel) const;
	bool CanAfford(int32 Cost) const { return CurrentMana >= Cost; }
	bool SpendMana(int32 Cost);
	void BroadcastState(int32 FamilyId);

	// -------------------------
	// Module Access Helpers
	// -------------------------
	UDataManager* GetDataManager() const;
	UUnitModule* GetUnitModule() const;
	UStageModule* GetStageModule() const;
	UStageInfoModule* GetStageInfoModule() const;

private:
	UPROPERTY()
	TArray<int32> AllyUnitIdList;

	UPROPERTY()
	TArray<int32> EnemyUnitIdList;

	UPROPERTY()
	TMap<int32, int32> SpawnLevels;

	UPROPERTY()
	TMap<int32, FUpgradeCostRule> CostRules;

	UPROPERTY()
	int32 MaxMana = 1000;

	UPROPERTY()
	int32 CurrentMana = 0;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnUpgrade")
	int32 DefaultSpawnLevel = 0;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnUpgrade")
	int32 DefaultEnemySpawnCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnUpgrade")
	int32 DefaultEnemyLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnUpgrade")
	int32 DefaultReserveSize = 30;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float DefaultSpawnCooltime = 10.0f;
};
