// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "RewardDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FRewardDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	ERewardType RewardType = ERewardType::None;
	UPROPERTY(EditAnywhere)
	int32 RewardDataId = 0;
	UPROPERTY(EditAnywhere)
	int32 Amount = 0;
};
