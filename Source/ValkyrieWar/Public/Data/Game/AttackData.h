// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/AttackDataRow.h"
#include "Data/Table/GameData/ProjectileDataRow.h"
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

	FORCEINLINE TSoftClassPtr<UAnimInstance> GetAnimInstance() { return TableData.AnimInstance; }
	FORCEINLINE TSoftObjectPtr<UAnimMontage> GetAnimMontage() { return TableData.AnimMontage; }
	FORCEINLINE TArray<USkillEffectData*> GetEffectList() { return EffectList; }
	FORCEINLINE FGameplayTag GetAbilityTag() { return TableData.AbilityTag; }
	FORCEINLINE FVector GetLocationOffset() { return TableData.PositionOffset; }
	FORCEINLINE FQuat GetRotatinOffset() { return FQuat(TableData.RotateOffset); }
	FORCEINLINE EAttackType GetAttackType() { return TableData.AttackType; }
	FORCEINLINE TArray<FGameplayCueData> const GetCue(EGameplayCueOrder InOrder)
	{
		TArray<FGameplayCueData> Data;
		for (const auto data : TableData.CueData)
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
	FAttackDataRow TableData;

	UPROPERTY()
	TArray<TObjectPtr<USkillEffectData>> EffectList;

	UPROPERTY()
	FProjectileDataRow ProjectileData;
};
