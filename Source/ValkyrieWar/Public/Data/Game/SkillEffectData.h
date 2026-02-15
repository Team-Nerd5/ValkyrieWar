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
	void MakeData(const FSkillEffectDataRow* InTableData);

	FORCEINLINE int32 GetGroupId() { return TableData->GroupId; }
	FORCEINLINE TArray<FEffectModifierData> GetModifiers() { return TableData->Modifiers; }
	FORCEINLINE EGameplayEffectDurationType GetDurationPolicy() { return TableData->DurationPolicy; }
	FORCEINLINE float GetDuration() { return TableData->Duration; }
	FORCEINLINE float GetApplyValue() { return TableData->ApplyValue; }
private:
	const FSkillEffectDataRow* TableData;
};
