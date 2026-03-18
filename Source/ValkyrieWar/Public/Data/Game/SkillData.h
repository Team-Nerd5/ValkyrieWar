// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "Data/Table/GameData/SkillDataRow.h"
#include "Data/Table/GameData/ProjectileDataRow.h"
#include "Data/Game/SkillEffectData.h"
#include "Data/Enum/CharacterEnums.h"
#include "SkillData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API USkillData : public UObject
{
	GENERATED_BODY()
public:
	void MakeData(const FSkillDataRow InTableData, UGameManager* InGameManager);

	FORCEINLINE TArray<USkillEffectData*> GetEffectList() { return EffectList; }
	FORCEINLINE FGameplayTag GetAbilityTag() { return TableData.AbilityTag; }
	FORCEINLINE TSoftObjectPtr<UAnimMontage> GetMontage() { return TableData.Montage; }
	FORCEINLINE EAttackType GetAttackType() { return TableData.AttackType; }
	FORCEINLINE TArray<FGameplayCueData> GetCue(EGameplayCueOrder InOrder)
	{
		TArray<FGameplayCueData> Data;
		for (auto data : TableData.CueData)
		{
			if (data.CueOrder == InOrder)
				Data.Add(data);
		}
		return Data;
	}
	FORCEINLINE FProjectileDataRow* const GetProjectileData()
	{
		if (TableData.ProjectileId <= 0) return nullptr;

		return &ProjectileData;
	}

private:
	UPROPERTY()
	FSkillDataRow TableData;
	UPROPERTY()
	TArray<TObjectPtr<USkillEffectData>> EffectList;
	UPROPERTY()
	FProjectileDataRow ProjectileData;
};
