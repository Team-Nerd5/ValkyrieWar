#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "Data/Enum/CommonEnums.h"
#include "BaseUnitSpawner.generated.h"

class UObjectPoolSubsystem;
class AUnitCharacter;
class UDataManager;
class UUnitData;

USTRUCT(BlueprintType)
struct FSinglePoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPoolTypes PoolType = EPoolTypes::None;

	// 테스트는 이 클래스를 그대로 스폰
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AUnitCharacter> UnitClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ReserveSize = 50;

	// UnitModule에서 찾아 SetData에 넣을 ID (테스트용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnitDataId = 0;
};

UCLASS()
class VALKYRIEWAR_API ABaseUnitSpawner : public ABaseActor
{
	GENERATED_BODY()

public:
	ABaseUnitSpawner();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StopSpawning();

	// 전투 업그레이드(현재는 외부에서 직접 호출해서 테스트 가능)
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SetSpawnCount(int32 InSpawnCount);

	UFUNCTION(BlueprintPure, Category = "Spawn")
	int32 GetSpawnCount() const { return SpawnCount; }

	// 유닛이 풀로 반환될 때(=OnRelease) 호출되면 Alive 리스트 즉시 정리
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void NotifyUnitReleased(AUnitCharacter* Unit);

	UFUNCTION(BlueprintPure, Category = "Spawn")
	int32 GetAliveCount() const { return AliveUnits.Num(); }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ---- 확장 포인트 ----
	// 나중에 SpawnUpgradeSubsystem 붙일 때 여기만 바꾸면 됨.
	virtual int32 ResolveSpawnDataId() const; // 기본: PoolEntry.UnitDataId
	virtual TSubclassOf<AUnitCharacter> ResolveSpawnClass(int32 ResolvedDataId) const; // 기본: PoolEntry.UnitClass

	// ---- Hook ----
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn|Hook")
	void BP_OnSpawningStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn|Hook")
	void BP_OnUnitSpawned(AUnitCharacter* Unit, EPoolTypes PoolType);

private:
	UFUNCTION()
	void HandleSpawnTick();

	UFUNCTION()
	void HandleCleanupTick();

	UFUNCTION()
	void HandleUnitDestroyed(AActor* DestroyedActor);

	bool TryInitPool();
	UObjectPoolSubsystem* GetPool() const;

	// SpawnUpgradeSubsystem 이벤트 수신 (승인된 업그레이드)
	UFUNCTION()
	void HandleSpawnLevelUpgraded(int32 InFamilyId, int32 OldLevel, int32 NewLevel);

	FTransform MakeSpawnTransform() const;

	void SpawnOne();
	UUnitData* ResolveUnitDataObject(int32 DataId) const;

	void RegisterAlive(AUnitCharacter* Unit);
	void UnregisterAlive(AActor* UnitActor);
	void CompactAliveUnits();

public:
	// ===== Pool =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool")
	FSinglePoolEntry PoolEntry;

	// ===== Spawn Transform =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<AActor> SpawnPointActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FVector2D SpawnHalfExtent = FVector2D(200.f, 200.f);

	// ===== Spawn Loop =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.01"))
	float SpawnInterval = 5.f;

	// NotifyUnitReleased를 못 받는 경우 대비(보험)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float CleanupInterval = 1.0f;

	// 한 틱에서 과도한 스폰 방지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 MaxSpawnPerTick = 10;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 UnitId = 0;

	UPROPERTY(EditDefaultsOnly)
	ETeam Team = ETeam::TeamA;

private:
	UPROPERTY(VisibleAnywhere, Category = "Spawn")
	int32 SpawnCount = 0;

	bool bPoolInited = false;

	FTimerHandle SpawnTickHandle;
	FTimerHandle CleanupHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<AUnitCharacter>> AliveUnits;
};
