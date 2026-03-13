// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Data/Table/GameData/UnitDataRow.h"
#include "Data/Table/GameData/StatGroupDataRow.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"

#include "GameplayTagContainer.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "Data/Enum/CharacterEnums.h"
#include "UnitData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UUnitData : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	uint64 UID = 0;

	UPROPERTY()
	FUnitDataRow TableData;

	UPROPERTY()
	TObjectPtr<UAttackData> AttackData = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<USkillData>> SkillData;

	UPROPERTY()
	TMap<EStatusType, float> Stat;

public:
	void MakeData(const FUnitDataRow InTableData, UGameManager* InGameManager);

	FORCEINLINE uint64 GetUID() { return UID; }
	FORCEINLINE TArray<USkillData*> GetSkillData() { return SkillData; }
	FORCEINLINE UAttackData* GetAttackData() { return AttackData; }
	FORCEINLINE float GetStat(EStatusType InType) { return *Stat.Find(InType); }
	FORCEINLINE ETeamType GetTeamType() { return TableData.TeamType; }
};
