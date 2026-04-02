// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Ability/SkillDamageExecCalc.h"
#include "Data/Attribute/StatAttributeSet.h"

struct SkillDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

    SkillDamageStatics()
    {
        // Snapshot=false: 맞을 당시의 실시간 스탯 사용
        DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, Attack, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, Defense, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, Health, Target, false);
    }
};

static const SkillDamageStatics& DamageStatics()
{
    static SkillDamageStatics Statics;
    return Statics;
}

USkillDamageExecCalc::USkillDamageExecCalc()
{
    RelevantAttributesToCapture.Add(DamageStatics().AttackDef);
    RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
    RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void USkillDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    // 태그 정보 가져오기 (필요시 사용)
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float AttackPower = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluationParameters, AttackPower);

    float Armor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters, Armor);
     
    //데미지 공식: Attack * (100 / (100 + Armor))
    float DamageReduction = (Armor >= 0.0f) ? (100.0f / (100.0f + Armor)) : 1.0f;
    float FinalDamage = AttackPower * DamageReduction;

    FGameplayTag DataTag = FGameplayTag::RequestGameplayTag(FName("Data.DamageIncrease"));
    float DamageIncrease = Spec.GetSetByCallerMagnitude(DataTag, false, 0.0f) * 0.01f + 1.0f;

    FinalDamage *= DamageIncrease;

    if (FinalDamage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FinalDamage));
    }
}
