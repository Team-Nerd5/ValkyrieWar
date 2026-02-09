// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Data/Enums.h"
#include "TestBaseUnit.generated.h"

class UTestUnitBrainComponent;
struct FTestEngagementSlot;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ATestBaseUnit : public ABaseCharacter
{
	GENERATED_BODY()

public:
    ATestBaseUnit();

    // 근/원거리 구분 (Auto 타깃팅 룰에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    EUnitCombatType CombatType = EUnitCombatType::Melee;

    // 이 유닛이 동시에 붙을 수 있게 허용하는 공격자 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    int32 MaxEngagementSlots = 2;

    // 공격 사거리 (간단 버전)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackRange = 120.f;

    // 공격 쿨다운
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackDamage = 10.f;

    // 체력(간단)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float MaxHP = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float CurrentHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool bDead = false;

    // 사망 후 파괴 시간(0이면 즉시 Destroy)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    float DestroyDelay = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UTestUnitBrainComponent> Brain;

    // 타깃이 나를 수용하는 슬롯 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TArray<FTestEngagementSlot> EngagementSlots;

    // 디버그용
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
    bool bDrawDebug = false;

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    bool IsDead() const { return CurrentHP <= 0.f; }

    // 슬롯 유틸
    int32 FindSlotOfAttacker(AActor* Attacker) const;
    int32 FindFirstFreeSlot() const;
    bool HasFreeSlot() const;

    // 실제 공격(간단)
    bool CanAttackNow(float Now) const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool PerformAttack(AActor* Target);

    // ===== Damage / Death =====
    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    void HandleDeath(AController* Killer, AActor* DamageCauser);

private:
    float LastAttackTime = -1000.f;

    FTimerHandle DestroyTimerHandle;
};
