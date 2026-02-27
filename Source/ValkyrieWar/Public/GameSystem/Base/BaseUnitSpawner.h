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

	// UnitModule에서 찾아서 SetData에 넣을 ID
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

	// 병종(옵션)별: 이 옵션이 뽑혔을 때 한 번에 몇 마리 스폰할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnCount = 1;
};

USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StartDelay = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnInterval = 2.f;

	// TotalToSpawn 삭제, "인구 유지형" 스폰: MaxAlive만 채움
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxAlive = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWaveOption> Options;
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
	void BP_OnUnitSpawned(AUnitCharacter* Unit, int32 WaveIndex, EPoolTypes PoolType);

private:
	// ---- Timer callbacks ----
	UFUNCTION()
	void HandleWaveStart();

	UFUNCTION()
	void HandleSpawnTick();

	UFUNCTION()
	void HandleCleanupTick();

	UFUNCTION()
	void HandleUnitDestroyed(AActor* DestroyedActor);

	UObjectPoolSubsystem* GetPool() const;
	const FPoolEntry* FindPoolEntry(EPoolTypes PoolType) const;

	void StartWaveInternal(int32 WaveIndex);

	// Option 자체를 가중치로 뽑아야 SpawnCount를 쓸 수 있음
	const FWaveOption* PickWeightedOption(const FWaveConfig& Wave) const;

	// 사각형 스폰
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

	// 원형 반경 → 사각형 Half Extent (스폰 포인트 기준 Forward/Right로 퍼짐)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FVector2D SpawnHalfExtent = FVector2D(200.f, 200.f);

	// ===== Waves =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FWaveConfig> Waves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bAutoStart = true;

	// 안전장치(NotifyUnitReleased를 못 받는 경우 대비)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float CleanupInterval = 1.0f;

private:
	int32 CurrentWaveIndex = INDEX_NONE;

	// 이번 Tick에 너무 많이 뽑지 않게 상한(성능 보호)
	UPROPERTY(EditAnywhere, Category = "Wave")
	int32 MaxSpawnCount = 10;

	FTimerHandle WaveStartHandle;
	FTimerHandle SpawnTickHandle;
	FTimerHandle CleanupHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<AUnitCharacter>> AliveUnits;
};
