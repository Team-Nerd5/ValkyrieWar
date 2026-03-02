// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ValkyrieSaveData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FValkyrieSaveData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int32 DataId = 0;

};
