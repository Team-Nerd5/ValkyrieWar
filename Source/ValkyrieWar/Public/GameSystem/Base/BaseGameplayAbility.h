// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/Table/GameData/SkillEffectDataRow.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillEffectData.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBaseGameplayAbility();

	/// <summary>
	/// TriggerEventData에 타겟 담아서 전달 필요
	/// </summary>
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	
	UFUNCTION()
	void ApplyAbilityToTarget(AActor* InTargetActor);

	//효과 데이터를 받아서 어빌리티 세팅.
	void UpdateData(FGameplayTag InAbilityTag, TArray<USkillEffectData*> InEffectDataList);
private:

	UPROPERTY()
	TArray<UGameplayEffect*> CachedEffects;
};
