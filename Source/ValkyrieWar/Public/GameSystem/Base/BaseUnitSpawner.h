// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "Data/Enum/CommonEnums.h"
#include "BaseUnitSpawner.generated.h"

class UObjectPoolSubsystem;
class AUnitCharacter;

USTRUCT(BlueprintType)
struct FPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPoolTypes PoolType = EPoolTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AUnitCharacter> UnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ReserveSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnitDataId = 0;
};

USTRUCT(BlueprintType)
struct FWaveOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPoolTypes PoolType = EPoolTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Weight = 1;
};

USTRUCT(BlueprintType)
struct FWaveConfig
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
	TArray<FWaveOption> Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EndDelay = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnCount = 1;
};

UCLASS()
class VALKYRIEWAR_API ABaseUnitSpawner : public ABaseActor
{
	GENERATED_BODY()

public:
	ABaseUnitSpawner();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWaves();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StopWaves();

	// 유닛이 풀로 반환될 때(=OnRelease) 호출해주면 AliveCount가 정확해짐
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void NotifyUnitReleased(AUnitCharacter* Unit);

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetAliveCount() const { return AliveUnits.Num(); }

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave|Hook")
	void BP_OnWaveStarted(int32 WaveIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave|Hook")
	void BP_OnWaveFinished(int32 WaveIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave|Hook")
	void BP_OnUnitSpawned(AUnitCharacter* Unit, int32 WaveIndex, EPoolTypes PoolType);

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

	UObjectPoolSubsystem* GetPool() const;
	const FPoolEntry* FindPoolEntry(EPoolTypes PoolType) const;

	void StartWaveInternal(int32 WaveIndex);
	void EndWaveInternal();

	EPoolTypes PickWeightedPoolType(const FWaveConfig& Wave) const;
	FTransform MakeSpawnTransform() const;

	void RegisterAlive(AUnitCharacter* Unit);
	void UnregisterAlive(AActor* UnitActor);
	void CompactAliveUnits();

public:
	// ===== Pool =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool")
	TArray<FPoolEntry> PoolEntries;

	// ===== Spawn Transform =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<AActor> SpawnPointActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float SpawnRadius = 200.f;

	// ===== Waves =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FWaveConfig> Waves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bLoopWaves = true;

	// 안전장치(NotifyUnitReleased를 못 받는 경우 대비)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float CleanupInterval = 1.0f;

private:
	int32 CurrentWaveIndex = INDEX_NONE;
	int32 SpawnedThisWave = 0;

	// 동시 소환 한계치 최대 10마리
	int32 MaxSpawnCount = 10;

	FTimerHandle WaveStartHandle;
	FTimerHandle SpawnTickHandle;
	FTimerHandle WaveEndHandle;
	FTimerHandle CleanupHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<AUnitCharacter>> AliveUnits;
};
