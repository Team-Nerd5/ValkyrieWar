// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/AttackDataRow.h"
#include "Data/Table/GameData/ProjectileDataRow.h"
#include "Data/Table/GameData/TargetingDataRow.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Game/SkillEffectData.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "AttackData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UAttackData : public UObject
{
	GENERATED_BODY()
public:
	void MakeData(const FAttackDataRow InTableData, UGameManager* InGameManager);

	FORCEINLINE TSoftObjectPtr<UAnimMontage> GetAnimMontage() { return TableData.AnimMontage; }
	FORCEINLINE TArray<USkillEffectData*> GetEffectList() { return EffectList; }
	FORCEINLINE FGameplayTag GetAbilityTag() { return TableData.AbilityTag; }
	FORCEINLINE EAttackType GetAttackType() { return TableData.AttackType; }
	FORCEINLINE TArray<FGameplayCueData> const GetCue(EGameplayCueOrder InOrder)
	{
		TArray<FGameplayCueData> OutData;
		for (const FGameplayCueData& Data : TableData.CueData)
		{
			if (Data.CueOrder == InOrder)
				OutData.Add(Data);
		}
		return OutData;
	}
	FORCEINLINE FProjectileDataRow* const GetProjectileData()
	{
		if (TableData.ProjectileId <= 0) return nullptr;

		return &ProjectileData;
	}

	FORCEINLINE FTargetingDataRow const GetTargetingData() { return TargetingData; }
	FORCEINLINE float const GetAttackRange() { return TableData.AttackRange; }
private:
	UPROPERTY()
	FAttackDataRow TableData;

	UPROPERTY()
	TArray<TObjectPtr<USkillEffectData>> EffectList;

	UPROPERTY()
	FTargetingDataRow TargetingData;

	UPROPERTY()
	FProjectileDataRow ProjectileData;
};
