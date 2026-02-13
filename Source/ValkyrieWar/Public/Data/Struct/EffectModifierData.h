// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "EffectModifierData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FEffectModifierData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Attribute;  // 바꿀 스탯 (Health, Mana...)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.0f;            // 수치 (-50, +20...)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGameplayModOp::Type> Op = EGameplayModOp::Additive;
};
