// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/UnitDataRow.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
#include "GameplayTagContainer.h"
#include "GameSystem/Instance/Game/GameManager.h"
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

	const FUnitDataRow* TableData = nullptr;

	UPROPERTY()
	TObjectPtr<UAttackData> AttackData = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<USkillData>> SkillData;

public:
	void MakeData(const FUnitDataRow* InTableData, UGameManager* InGameManager);

	FORCEINLINE uint64 GetUID() { return UID; }
	FORCEINLINE TArray<USkillData*> GetSkillData() { return SkillData; }
	FORCEINLINE UAttackData* GetAttackData() { return AttackData; }
};
