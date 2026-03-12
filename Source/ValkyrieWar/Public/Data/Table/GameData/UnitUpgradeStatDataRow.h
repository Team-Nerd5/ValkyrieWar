// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "Data/Enum/CharacterEnums.h"
#include "UnitUpgradeStatDataRow.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FUnitUpgradeStatDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 GroupId = 0;
	UPROPERTY(EditAnywhere)
	EGoodsType UpgradeCostType = EGoodsType::Gold;
	UPROPERTY(EditAnywhere)
	int32 UpgradeCost = 0;
	UPROPERTY(EditAnywhere)
	int32 Level = 1;
	UPROPERTY(EditAnywhere)
	int32 StatGroupId = 0;
};
