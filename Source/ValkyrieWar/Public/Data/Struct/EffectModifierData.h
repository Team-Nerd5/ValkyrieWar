// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "EffectModifierData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FEffectModifierData
{
	GENERATED_BODY()

    /// <summary>
    /// 타겟에 적용할 Attribute
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayAttribute TargetAttribute;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EGameplayModOp::Type> Op = EGameplayModOp::Additive;

    /// <summary>
    /// 값을 계산할 기반 Attribute
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayAttribute SourceAttribute;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseSourceAttribute = false;
};
