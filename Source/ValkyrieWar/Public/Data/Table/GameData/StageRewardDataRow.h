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
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	int32 StageChapter = 0;
	UPROPERTY(EditAnywhere)
	int32 StarAmount = 0;
	UPROPERTY(EditAnywhere)
	int32 RewardId = 0;

};
