// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SkillDamageExecCalc.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API USkillDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	USkillDamageExecCalc();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
