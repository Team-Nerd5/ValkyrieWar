// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Interface/ObjectPool/ObjectPoolInterface.h"
#include "Data/Enum/CommonEnums.h"
#include "TestBaseUnit.generated.h"

class ATestUnitSpawner;
class UTestUnitBrainComponent;
class UTestBattleDirectorSubsystem;
class UObjectPoolSubsystem;
struct FTestEngagementSlot;

UCLASS()
class VALKYRIEWAR_API ATestBaseUnit : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ATestBaseUnit();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	EUnitCombatType CombatType = EUnitCombatType::Melee;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MaxEngagementSlots = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 160.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float MaxHP = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float CurrentHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float DestroyDelay = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTestUnitBrainComponent> Brain;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<FTestEngagementSlot> EngagementSlots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDrawDebug = false;

	void SetOwnerSpawner(ATestUnitSpawner* InSpawner);

	// 스포너가 스폰(또는 재사용) 시점에 세팅
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPoolType(EPoolTypes InType) { MyPoolType = InType; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	EPoolTypes MyPoolType = EPoolTypes::None;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool IsDead() const { return CurrentHP <= 0.f; }

	int32 FindSlotOfAttacker(AActor* Attacker) const;
	int32 FindFirstFreeSlot() const;
	bool HasFreeSlot() const;

	bool CanAttackNow(float Now) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformAttack(AActor* Target);

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	void HandleDeath(AController* Killer, AActor* DamageCauser);

	// ===== Pool Hooks =====
	virtual void OnGet_Implementation() override;
	virtual void OnRelease_Implementation() override;

private:
	// ---- BattleDirector 연동 ----
	UTestBattleDirectorSubsystem* GetBattleDirector() const;
	void RegisterToBattleDirector();
	void UnregisterFromBattleDirector(bool bReleaseAttackersIfIAmTarget);

	// ---- Pool ----
	UObjectPoolSubsystem* GetPool() const;

	// 풀 재사용 초기화
	void ResetForReuse();

protected:


private:
	UPROPERTY()
	TWeakObjectPtr<ATestUnitSpawner> OwnerSpawner;

	float LastAttackTime = -1000.f;

	FTimerHandle DestroyTimerHandle;

	bool bRegisteredToBattleDirector = false;
};
