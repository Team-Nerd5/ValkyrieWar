// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseGameplayAbility.h"
#include "GameSystem/Ability/SkillDamageExecCalc.h"

#include "GameplayEffect.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"
#include "GameplayEffectTypes.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Data/Game/SkillData.h"

UBaseGameplayAbility::UBaseGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

    if (TriggerEventData && TriggerEventData->Target)
    {
        // 이벤트로 전달받은 타겟 사용
        const AActor* TargetActor = TriggerEventData->Target;
        ApplyAbilityToTarget(const_cast<AActor*>(TargetActor));
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UBaseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnAvatarSet(ActorInfo, Spec);

    UAttackData* InAttackData = Cast<UAttackData>(GetCurrentSourceObject());
    if (InAttackData)
    {
        UpdateData(InAttackData->GetAbilityTag(), InAttackData->GetEffectList());
    }

    USkillData* SkillData = Cast<USkillData>(GetCurrentSourceObject());
    if (SkillData)
    {
        UpdateData(SkillData->GetAbilityTag(), SkillData->GetEffectList());
    }
}

void UBaseGameplayAbility::ApplyAbilityToTarget(AActor* InTargetActor)
{
    if (!InTargetActor || CachedEffects.Num() <= 0) return;

    //타겟의 ASC 가져오기
    UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetActor);

    if (!MyASC || !TargetASC) return;

    FGameplayEffectContextHandle Context = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);

    for (int32 i = 0; i < CachedEffects.Num(); i++)
    {
        UGameplayEffect* Effect = CachedEffects[i];

        if (!Effect) continue;

        FGameplayEffectSpec Spec(Effect, Context, 1.0f);

        //데미지 증가율 저장
        FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.DamageIncrease"));
        Spec.SetSetByCallerMagnitude(DamageTag, CachedDamagePer[i]);
        //적용
        TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
    }
    for (UGameplayEffect* Effect : CachedEffects)
    {
        
    }
}

void UBaseGameplayAbility::UpdateData(FGameplayTag InAbilityTag, TArray<USkillEffectData*> InEffectDataList)
{
    CachedEffects.Empty();

    if (InAbilityTag.IsValid())
    {
        FAbilityTriggerData TriggerData;
        TriggerData.TriggerTag = InAbilityTag;

        AbilityTriggers.Add(TriggerData);
    }

    //CachedEffect 세팅
    for (USkillEffectData* EffectData : InEffectDataList)
    {
        FString Name = FString::Printf(TEXT("GE_Cache_%d"), CachedEffects.Num());
        UGameplayEffect* NewEffect = NewObject<UGameplayEffect>(this, FName(*Name));

        NewEffect->DurationPolicy = EffectData->GetDurationPolicy();
        if (NewEffect->DurationPolicy == EGameplayEffectDurationType::HasDuration)
        {
            NewEffect->DurationMagnitude = FScalableFloat(EffectData->GetDuration());
        }
        if (EffectData->GetPeriod() > 0.0f)
        {
            NewEffect->Period.Value = EffectData->GetPeriod();
        }

        //태그부여
        if (EffectData->GetGrantedTags().IsValid())
        {
            NewEffect->CachedGrantedTags = EffectData->GetGrantedTags();
        }

        if (EffectData->UseCalc())
        {
            FGameplayEffectExecutionDefinition ExecDef;
            ExecDef.CalculationClass = USkillDamageExecCalc::StaticClass();
            NewEffect->Executions.Add(ExecDef);
        }
        else
        {
            // [단순 계산] Modifier 연결
            int32 Idx = NewEffect->Modifiers.Num();
            NewEffect->Modifiers.Add(FGameplayModifierInfo());
            FGameplayModifierInfo& ModInfo = NewEffect->Modifiers[Idx];

            ModInfo.Attribute = EffectData->GetTargetAttribute();
            ModInfo.ModifierOp = EffectData->GetOp();

            if (EffectData->UseSourceAttribute() && EffectData->GetSourceAttribute().IsValid())
            {
                // 스탯 비례
                FAttributeBasedFloat AttributeBasedFloat;
                AttributeBasedFloat.BackingAttribute.AttributeToCapture = EffectData->GetSourceAttribute();
                AttributeBasedFloat.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
                AttributeBasedFloat.BackingAttribute.bSnapshot = false;
                AttributeBasedFloat.Coefficient = FScalableFloat(EffectData->GetApplyValue());
                ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(AttributeBasedFloat);
            }
            else
            {
                // 고정 수치
                ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(EffectData->GetApplyValue()));
            }
        }

        //캐시 목록에 추가
        CachedEffects.Add(NewEffect);        
        CachedDamagePer.Add(EffectData->GetDamagePec());
    }
}
