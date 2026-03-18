#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "Data/Enum/CommonEnums.h"
#include "Data/Struct/ComputedEnemyStat.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "BaseUnitSpawner.generated.h"

class UObjectPoolSubsystem;
//class AUnitCharacter;
class UUnitData;

USTRUCT(BlueprintType)
struct FSpawnUnitEntry
{
	GENERATED_BODY()

	// 현재는 FamilyId == UnitDataId 로 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 FamilyId = 0;

	// 실제 스폰할 유닛 데이터 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 UnitDataId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	EPoolTypes PoolType = EPoolTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<AUnitCharacter> UnitClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "1"))
	int32 ReserveSize = 30;

	// 매 스폰 주기마다 생성할 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0"))
	int32 SpawnCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnitLevel = 1;

	UPROPERTY()
	bool bUseComputedStat = false;

	UPROPERTY()
	FComputedEnemyStat ComputedStat;
};

UCLASS()
class VALKYRIEWAR_API ABaseUnitSpawner : public ABaseActor
{
	GENERATED_BODY()

public:
	ABaseUnitSpawner();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SetSpawnEntries(const TArray<FSpawnUnitEntry>& InEntries);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void ClearSpawnEntries();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void UpdateEntrySpawnCount(int32 InFamilyId, int32 InSpawnCount);

	UFUNCTION(BlueprintPure, Category = "Spawn")
	const TArray<FSpawnUnitEntry>& GetSpawnEntries() const { return SpawnEntries; }

	UFUNCTION(BlueprintPure, Category = "Spawn")
	int32 GetAliveCount() const { return SpawnedUnits.Num(); }

	UFUNCTION(BlueprintPure, Category = "Spawn")
	int32 GetSpawnerId() const { return SpawnerId; }

	UFUNCTION(BlueprintPure, Category = "Spawn")
	ETeamType GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StopSpawning();

	// 유닛이 풀로 반환될 때 호출
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void NotifyUnitReleased(AUnitCharacter* Unit);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn|Hook")
	void BP_OnSpawningStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn|Hook")
	void BP_OnUnitSpawned(AUnitCharacter* Unit, int32 FamilyId, int32 UnitDataId, EPoolTypes PoolType);

private:
	UFUNCTION()
	void HandleSpawnTick();

	UFUNCTION()
	void HandleCleanupTick();

	UFUNCTION()
	void HandleUnitDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void RequestUnitDataToSpawn();

	UFUNCTION()
	void HandleSpawnLevelUpgraded(int32 InFamilyId, int32 OldLevel, int32 NewLevel);

	bool TryInitPools();
	bool TryInitPoolForEntry(const FSpawnUnitEntry& Entry);

	UObjectPoolSubsystem* GetPool() const;
	UUnitData* ResolveUnitDataObject(int32 DataId) const;

	void SpawnOne(const FSpawnUnitEntry& Entry);
	FTransform MakeSpawnTransform() const;

	void RegisterSpawnedUnit(AUnitCharacter* Unit);
	void UnregisterSpawnedUnit(AActor* UnitActor);
	void CompactSpawnedUnits();

	int32 FindEntryIndexByFamilyId(int32 InFamilyId) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 SpawnerId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<AActor> SpawnPointActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FVector2D SpawnHalfExtent = FVector2D(200.f, 3000.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.01"))
	float SpawnInterval = 10.0f;

	// NotifyUnitReleased 못 받는 경우 대비
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float CleanupInterval = 1.0f;

	// 한 틱에 너무 많이 생성되는 것 방지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 MaxSpawnPerTick = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	ETeamType Team = ETeamType::Ally;

private:
	UPROPERTY(VisibleAnywhere, Category = "Spawn")
	TArray<FSpawnUnitEntry> SpawnEntries;

	// PoolType별 초기화 여부/클래스 충돌 검사
	UPROPERTY()
	TMap<EPoolTypes, TSubclassOf<AUnitCharacter>> InitedPools;

	FTimerHandle SpawnTickHandle;
	FTimerHandle CleanupHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<AUnitCharacter>> SpawnedUnits;
};
