// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StageRewardDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStageRewardDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 GroupId = 0;
	UPROPERTY(EditAnywhere)
	int32 RewardId = 0;
	UPROPERTY(EditAnywhere)
	int32 RewardRate = 0;		//만분율
};
