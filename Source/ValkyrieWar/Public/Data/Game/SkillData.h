// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "Data/Table/GameData/SkillDataRow.h"
#include "Data/Game/SkillEffectData.h"
#include "SkillData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API USkillData : public UObject
{
	GENERATED_BODY()
public:
	void MakeData(const FSkillDataRow* InTableData, UGameManager* InGameManager);

	FORCEINLINE TArray<USkillEffectData*> GetEffectList() { return EffectList; }
private:
	const FSkillDataRow* TableData;

	TArray<USkillEffectData*> EffectList;
};
