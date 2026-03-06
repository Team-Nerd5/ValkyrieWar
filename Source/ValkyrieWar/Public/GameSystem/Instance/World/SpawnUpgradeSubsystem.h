#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
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

/**
 * 업그레이드 권위(마나/레벨/룰) + UI상태 브로드캐스트 + 스포너에 승인 통지(Delegate)
 */
UCLASS()
class VALKYRIEWAR_API USpawnUpgradeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void BindUpgradeDelegates();
	void UnbindUpgradeDelegates();

	// ===== Query =====
	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	int32 GetSpawnLevel(int32 InFamilyId) const;

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	int32 GetCurrentMana() const { return CurrentMana; }

	// ===== Authority Setters =====
	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void SetCurrentMana(int32 InMana, bool bBroadcastAll = true);

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void AddMana(int32 InDeltaMana);

	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void SetCostRule(int32 InFamilyId, const FUpgradeCostRule& Rule, bool bBroadcast = true);

	UFUNCTION()
	void HandleUpgradeClicked(int32 InFamilyId);

	UFUNCTION(BlueprintCallable)
	void EnsureFamily(int32 InFamilyId, int32 InDefaultLevel = 0, bool bBroadcast = true);

	// 전 카드(UI) 상태 동기화
	UFUNCTION(BlueprintCallable, Category = "SpawnUpgrade")
	void SyncAll();

private:
	UPROPERTY()
	TMap<int32, int32> SpawnLevels;

	UPROPERTY()
	TMap<int32, FUpgradeCostRule> CostRules;

	UPROPERTY()
	int32 CurrentMana = 0;

	int32 DefaultSpawnLevel = 0;

	// ===== Internals =====
	void InitFamilyIfNeeded(int32 InFamilyId, int32 InDefaultLevel = 0);

	int32 CalcCost(int32 FamilyId, int32 CurrentLevel) const;

	bool CanAfford(int32 Cost) const { return CurrentMana >= Cost; }
	bool SpendMana(int32 Cost); // 성공 시 차감

	void BroadcastState(int32 FamilyId); // UI용 상태
};
