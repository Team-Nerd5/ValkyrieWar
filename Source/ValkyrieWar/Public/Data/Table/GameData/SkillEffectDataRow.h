// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "Data/Struct/EffectModifierData.h"
#include "GameplayEffect.h"
#include "SkillEffectDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FSkillEffectDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId;
	UPROPERTY(EditAnywhere)
	int32 GroupId;
	UPROPERTY(EditAnywhere)
	ESkillType SkillType;
	UPROPERTY(EditAnywhere)
	TArray<FEffectModifierData> Modifiers;
	UPROPERTY(EditAnywhere)
	EAttributeType AttributeType;
	UPROPERTY(EditAnywhere)
	EGameplayEffectDurationType DurationPolicy;
	UPROPERTY(EditAnywhere)
	float Duration = 0.0f;
	UPROPERTY(EditAnywhere)
	float ApplyValue = 0.0f;
	UPROPERTY(EditAnywhere)
	ESkillApplyType ApplyType;
	UPROPERTY(EditAnywhere)
	ETargetType TargetType;
	UPROPERTY(EditAnywhere)
	int32 TargetAmount = 1;
};
