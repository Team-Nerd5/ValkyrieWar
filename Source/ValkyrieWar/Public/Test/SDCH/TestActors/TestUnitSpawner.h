#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enums.h"
#include "TestUnitSpawner.generated.h"

class UObjectPoolSubsystem;
class ATestBaseUnit;

USTRUCT(BlueprintType)
struct FTestPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPoolTypes PoolType = EPoolTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ATestBaseUnit> UnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ReserveSize = 50;
};

USTRUCT(BlueprintType)
struct FTestWaveOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPoolTypes PoolType = EPoolTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Weight = 1;
};

USTRUCT(BlueprintType)
struct FTestWaveConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StartDelay = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnInterval = 2.f;

	// <= 0이면 무한 스폰
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TotalToSpawn = 20;

	// 이 스포너 기준 동시 생존 최대
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxAlive = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTestWaveOption> Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EndDelay = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnCount = 1;
};

UCLASS()
class VALKYRIEWAR_API ATestUnitSpawner : public AActor
{
	GENERATED_BODY()

public:
	ATestUnitSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ===== Pool =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool")
	TArray<FTestPoolEntry> PoolEntries;

	// ===== Spawn Transform =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<AActor> SpawnPointActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float SpawnRadius = 200.f;

	// ===== Waves =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FTestWaveConfig> Waves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bLoopWaves = true;

	// 안전장치(NotifyUnitReleased를 못 받는 경우 대비)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float CleanupInterval = 1.0f;

public:
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWaves();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StopWaves();

	// 유닛이 풀로 반환될 때(=OnRelease) 호출해주면 AliveCount가 정확해짐
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void NotifyUnitReleased(ATestBaseUnit* Unit);

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetAliveCount() const { return AliveUnits.Num(); }

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Wave|Hook")
	void BP_OnWaveStarted(int32 WaveIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave|Hook")
	void BP_OnWaveFinished(int32 WaveIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave|Hook")
	void BP_OnUnitSpawned(ATestBaseUnit* Unit, int32 WaveIndex, EPoolTypes PoolType);

private:
	// ---- Timer callbacks (람다 없음) ----
	UFUNCTION()
	void HandleWaveStart();

	UFUNCTION()
	void HandleSpawnTick();

	UFUNCTION()
	void HandleWaveEnd();

	UFUNCTION()
	void HandleCleanupTick();

	UFUNCTION()
	void HandleUnitDestroyed(AActor* DestroyedActor);

private:
	UObjectPoolSubsystem* GetPool() const;
	const FTestPoolEntry* FindPoolEntry(EPoolTypes PoolType) const;

	void StartWaveInternal(int32 WaveIndex);
	void EndWaveInternal();

	EPoolTypes PickWeightedPoolType(const FTestWaveConfig& Wave) const;
	FTransform MakeSpawnTransform() const;

	void RegisterAlive(ATestBaseUnit* Unit);
	void UnregisterAlive(AActor* UnitActor);
	void CompactAliveUnits();

	// 풀 반환 상태(휴리스틱) 판정: Notify 누락 대비
	bool IsReturnedToPoolHeuristic(const ATestBaseUnit* Unit) const;

private:
	int32 CurrentWaveIndex = INDEX_NONE;
	int32 SpawnedThisWave = 0;

	FTimerHandle WaveStartHandle;
	FTimerHandle SpawnTickHandle;
	FTimerHandle WaveEndHandle;
	FTimerHandle CleanupHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<ATestBaseUnit>> AliveUnits;
};
