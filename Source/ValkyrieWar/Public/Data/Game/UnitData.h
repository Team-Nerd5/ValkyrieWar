// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Data/Table/GameData/UnitDataRow.h"
#include "Data/Table/GameData/StatGroupDataRow.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
#include "Data/Struct/StatValueData.h"

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

	UPROPERTY()
	EGradeType CurrentGrade = EGradeType::Common;

	UPROPERTY()
	int32 Level = 1;

public:
	void MakeData(const FUnitDataRow InTableData, UGameManager* InGameManager);

	void LoadData(uint64 InUID, const FUnitDataRow InTableData, int32 InLevel, UGameManager* InGameManager);

	void LevelUp();

	FORCEINLINE uint64 GetUID() { return UID; }
	FORCEINLINE FUnitDataRow GetTableData() { return TableData; }
	FORCEINLINE int32 GetDataId() { return TableData.DataId; }
	FORCEINLINE TSoftObjectPtr<UTexture2D> GetIcon() { return TableData.UnitIcon; }
	FORCEINLINE EUnitCharacterType GetUnitType() { return TableData.UnitType; }
	FORCEINLINE int32 GetLevelUpGroupId() { return TableData.LevelUpGroupId; }
	FORCEINLINE TArray<USkillData*> GetSkillData() { return SkillData; }
	FORCEINLINE UAttackData* GetAttackData() { return AttackData; }
	FORCEINLINE float GetStat(EStatusType InType)
	{
		if (Stat.Contains(InType))
			return Stat.FindChecked(InType);
		else
			return 0.0f;
	}
	FORCEINLINE ETeamType GetTeamType() { return TableData.TeamType; }
	FORCEINLINE int32 GetLevel() { return Level; }
	FORCEINLINE EGradeType GetCurrentGrade() { return CurrentGrade; }
};
