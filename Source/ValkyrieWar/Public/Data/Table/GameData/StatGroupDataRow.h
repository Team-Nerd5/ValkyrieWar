// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatGroupDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStatGroupDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	float Attack = 0.0f;
	UPROPERTY(EditAnywhere)
	float Health = 0.0f;
	UPROPERTY(EditAnywhere)
	float Defence = 0.0f;
};
