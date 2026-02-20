// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Data/Game/UnitData.h"
#include "Test/SDCH/TestInterface/TestTargetReservationInterface.h"
#include "UnitCharacter.generated.h"

class UBattleDirectorSubsystem;
class UObjectPoolSubsystem;
class ABaseUnitSpawner;
class UUnitBrainComponent;
struct FUnitEngagementSlotData;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AUnitCharacter : public ABaseCharacter, public ITestTargetReservationInterface
{
	GENERATED_BODY()

public:
	AUnitCharacter();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//캐릭터 생성 시 데이터 세팅
	void SetData(UUnitData* InData);

	bool IsDead() const { return bDead; }

	void SetOwnerSpawner(ABaseUnitSpawner* InSpawner);

	// 스포너가 스폰(또는 재사용) 시점에 세팅
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPoolType(EPoolTypes InType) { MyPoolType = InType; }

	inline bool IsInPool() { return bInPool; }

	int32 FindSlotOfAttacker(AActor* Attacker) const;
	int32 FindFirstFreeSlot() const;

	bool HasFreeSlot() const;

	bool CanAttackNow(float Now) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformAttack(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyAttackDamage(AActor* Target);

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

	virtual void OnTargetAssigned_Implementation(AActor* NewTarget) override;

	// 공격 성공 시 호출해서 카운트다운 리셋
	void ResetStuckCountdown(bool bAlsoClearBB = true);

private:
	// ---- BattleDirector 연동 ----
	UBattleDirectorSubsystem* GetBattleDirector() const;
	void RegisterToBattleDirector();
	void UnregisterFromBattleDirector(bool bReleaseAttackersIfIAmTarget);

	// ---- Pool ----
	UObjectPoolSubsystem* GetPool() const;

	// 풀 재사용 초기화
	void ResetForReuse();

	// 내부 적용 함수
	void ApplyMoveSpeed(float NewSpeed);

	void StartStuckMonitor();
	void StopStuckMonitor();

	void StuckMonitorTick();

	void SetNeedToEscapeBB(bool bValue);

public:
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


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float DestroyDelay = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UUnitBrainComponent> Brain;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<FUnitEngagementSlotData> EngagementSlots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDrawDebug = false;

	// ===== Speed tuning =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed")
	float WalkSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed")
	float RunSpeed = 600.f;

	// 예약 타깃이 있을 때는 달리고, 없으면 걷기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed")
	bool bRunWhenHasTarget = true;

	// ===== Stuck / Escape =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Escape")
	bool bEnableEscapeWhenStuck = true;

	// 타이머 체크 주기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Escape")
	float StuckCheckInterval = 0.5f;

	// 이 시간 동안 "거의 정지" + Moving이면 탈출 요청
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Escape")
	float StuckTimeoutSeconds = 1.f;

	// 속도(2D)가 이 값보다 작으면 "정지로 간주"
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Escape")
	float StuckSpeedThreshold = 5.f; // cm/s

	// 탈출 요청 쿨다운 (연타 방지)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Escape")
	float EscapeRequestCooldown = 1.5f;

	// BB 키 이름(프로젝트에 맞게 그대로 쓰기)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Escape")
	FName BB_NeedToEscapeKey = TEXT("NeedToEscape");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	EPoolTypes MyPoolType = EPoolTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

protected:
	UPROPERTY()
	TObjectPtr<UUnitData> Data = nullptr;

private:
	UPROPERTY(VisibleAnywhere, Category = "Health")
	bool bDead = false;

	UPROPERTY()
	TWeakObjectPtr<ABaseUnitSpawner> OwnerSpawner;

	float LastAttackTime = -1000.f;

	FTimerHandle DestroyTimerHandle;

	bool bRegisteredToBattleDirector = false;
	bool bInPool;

	UPROPERTY()
	TWeakObjectPtr<AActor> LastAssignedTarget;

	FTimerHandle StuckMonitorTimerHandle;

	float StuckAccumSeconds = 0.f;
	float LastEscapeRequestTime = -10000.f;
};
