// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/SkillEffectDataRow.h"
#include "SkillEffectData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API USkillEffectData : public UObject
{
	GENERATED_BODY()

public:
	void MakeData(const FSkillEffectDataRow InTableData);

	FORCEINLINE int32 GetGroupId() { return TableData.GroupId; }
	FORCEINLINE EGameplayEffectDurationType GetDurationPolicy() { return TableData.DurationPolicy; }
	FORCEINLINE float GetDuration() { return TableData.Duration; }
	FORCEINLINE float GetApplyValue() { return TableData.Value; }
	FORCEINLINE float GetPeriod() { return TableData.Period; }
	FORCEINLINE FGameplayTagContainer GetGrantedTags() { return TableData.GrantedTags; }
	FORCEINLINE FGameplayTag GetCueTag() { return TableData.GameplayCueTag; }
	FORCEINLINE bool UseCalc() { return TableData.bUseDamageCalc; }
	FORCEINLINE FGameplayAttribute GetTargetAttribute() { return TableData.TargetAttribute; }
	FORCEINLINE TEnumAsByte<EGameplayModOp::Type> GetOp() { return TableData.Op; }
	FORCEINLINE bool UseSourceAttribute() { return TableData.bUseSourceAttribute; }
	FORCEINLINE FGameplayAttribute GetSourceAttribute() { return TableData.SourceAttribute; }

private:
	UPROPERTY()
	FSkillEffectDataRow TableData;
};
