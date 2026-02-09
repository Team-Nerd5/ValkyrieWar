// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StageInfoDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStageInfoDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 GroupId;
	UPROPERTY(EditAnywhere)
	int32 EnemyUnit1;
	UPROPERTY(EditAnywhere)
	int32 EnemyUnit2;
	UPROPERTY(EditAnywhere)
	int32 EnemyUnit3;
	UPROPERTY(EditAnywhere)
	int32 EnemyUnit4;
	UPROPERTY(EditAnywhere)
	int32 EnemyUnit5;
	UPROPERTY(EditAnywhere)
	int32 RewardGroupId;

};
