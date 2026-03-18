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
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	int32 GroupId = 0;
	UPROPERTY(EditAnywhere)
	ESkillType SkillType = ESkillType::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGameplayEffectDurationType DurationPolicy = EGameplayEffectDurationType::Instant;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Period = 0.0f; // 0보다 크면 도트(Dot) 효과

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer GrantedTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseDamageCalc = false;

    // 수치 설정 (Modifier용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseDamageCalc==false"))
    FGameplayAttribute TargetAttribute; // 바뀔 타겟 스탯

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseDamageCalc==false"))
    TEnumAsByte<EGameplayModOp::Type> Op = EGameplayModOp::Additive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseDamageCalc==false"))
    float Value = 0.0f;

    // 스탯 비례 여부 (내 공격력의 N% 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseSourceAttribute = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseSourceAttribute==true"))
    FGameplayAttribute SourceAttribute; // 기준 스탯
};
