// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestUnitSpawner.generated.h"

class UObjectPoolSubsystem;
class AActor;
class ABaseCharacter;

USTRUCT(BlueprintType)
struct FTestPoolSpawnEntry
{
	GENERATED_BODY()

	// "클래스마다 고유한 PoolType"을 써야 함 (현재 풀 구현 제약)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPoolTypes PoolType;

	// 풀링할 유닛 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABaseCharacter> UnitClass;

	// Reserve 용량 (실제 Max 제한이 아니라 Reserve임)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ReserveSize = 50;
};

UCLASS()
class VALKYRIEWAR_API ATestUnitSpawner : public AActor
{
	GENERATED_BODY()

public:
	ATestUnitSpawner();

protected:
	virtual void BeginPlay() override;

public:
	// ====== 설정 ======
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Pool")
	TArray<FTestPoolSpawnEntry> PoolEntries;

	// 스폰 기준 트랜스폼(비워두면 스포너 자신의 트랜스폼 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Spawn")
	TObjectPtr<AActor> SpawnPointActor = nullptr;

	// 스폰 오프셋(원형 랜덤)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Spawn")
	float SpawnRadius = 200.f;

	// ====== API ======
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	ABaseCharacter* SpawnFromPool(EPoolTypes PoolType);

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void DespawnToPool(EPoolTypes PoolType, ABaseCharacter* Unit);

	// 특정 PoolType의 클래스를 찾기
	const FTestPoolSpawnEntry* FindEntry(EPoolTypes PoolType) const;

protected:
	// 스폰 직후 유닛 세팅 훅(팀 설정/브레인 초기화/BT 재시작 등)
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawner|Hook")
	void BP_OnUnitSpawned(ABaseCharacter* Unit);

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawner|Hook")
	void BP_OnUnitDespawned(ABaseCharacter* Unit);

private:
	UObjectPoolSubsystem* GetPoolSubsystem() const;
	FTransform GetSpawnTransform() const;

};
