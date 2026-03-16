// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/UnitUpgradeStatDataRow.h"
#include "Data/Table/GameData/StatGroupDataRow.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "UnitUpgradeData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UUnitUpgradeData : public UObject
{
	GENERATED_BODY()
public:
	void MakeData(const FUnitUpgradeStatDataRow InTableData, UGameManager* InGameManager);

	FORCEINLINE int32 GetAttack() { return StatData.Attack; }
	FORCEINLINE int32 GetDefence() { return StatData.Defence; }
	FORCEINLINE int32 GetHealth() { return StatData.Health; }
protected:
	UPROPERTY()
	FUnitUpgradeStatDataRow TableData;

	UPROPERTY()
	FStatGroupDataRow StatData;
};
