// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseGameplayAbility.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

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

void UBaseGameplayAbility::ApplyAbilityToTarget(AActor* InTargetActor)
{
    if (!InTargetActor || CachedEffects.Num() <= 0) return;

    // 1. 타겟의 ASC 가져오기 (GAS가 없는 적일 수도 있으니 체크)
    UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetActor);

    if (!MyASC || !TargetASC) return;

    FGameplayEffectContextHandle Context = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);

    for (UGameplayEffect* Effect : CachedEffects)
    {
        if (!Effect) continue;       

        // Spec(명세서) 만들기    //1.0은 레벨..
        FGameplayEffectSpec Spec(Effect, Context, 1.0f);

        //적용
        TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
    }
}

void UBaseGameplayAbility::UpdateData(TArray<USkillEffectData*> InEffectDataList)
{
    CachedEffects.Empty();

    //CachedEffect 세팅
    for (USkillEffectData* EffectData : InEffectDataList)
    {
        UGameplayEffect* NewEffect = NewObject<UGameplayEffect>(this);

        // 2. 지속 시간 설정
        NewEffect->DurationPolicy = EffectData->GetDurationPolicy();

        // 지속 시간이 있는 경우 설정
        if (NewEffect->DurationPolicy == EGameplayEffectDurationType::HasDuration)
        {
            NewEffect->DurationMagnitude = FScalableFloat(EffectData->GetDuration());
        }

        //Attribute랑 값 세팅(여러개 가능)
        for (const FEffectModifierData& ModData : EffectData->GetModifiers())
        {
            int32 Idx = NewEffect->Modifiers.Num();
            NewEffect->Modifiers.Add(FGameplayModifierInfo());
            FGameplayModifierInfo& ModInfo = NewEffect->Modifiers[Idx];

            ModInfo.Attribute = ModData.TargetAttribute;
            ModInfo.ModifierOp = ModData.Op;

            if (ModData.bUseSourceAttribute && ModData.SourceAttribute.IsValid())
            {
                FAttributeBasedFloat AttributeBasedFloat;
                AttributeBasedFloat.BackingAttribute.AttributeToCapture = ModData.SourceAttribute;
                AttributeBasedFloat.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
                AttributeBasedFloat.BackingAttribute.bSnapshot = false;
                AttributeBasedFloat.Coefficient = FScalableFloat(ModData.Value);

                ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(AttributeBasedFloat);
            }
            else
            {
                ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(ModData.Value));
            }
        }

        // 4. 캐시 목록에 추가
        CachedEffects.Add(NewEffect);
    }
}
