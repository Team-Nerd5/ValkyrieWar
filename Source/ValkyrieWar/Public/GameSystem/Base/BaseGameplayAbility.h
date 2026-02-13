// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/Table/GameData/SkillEffectDataRow.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	/// <summary>
	/// TriggerEventData에 타겟 담아서 전달 필요
	/// </summary>
	/// <param name="Handle"></param>
	/// <param name="ActorInfo"></param>
	/// <param name="ActivationInfo"></param>
	/// <param name="TriggerEventData"></param>
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//타겟이 성벽일 수도 있으니 액터로
	UFUNCTION()
	void ApplyAbilityToTarget(AActor* InTargetActor);

	void UpdateData(TArray<FSkillEffectDataRow> InSkillEffects);

private:

	UPROPERTY()
	TArray<UGameplayEffect*> CachedEffects;
};
