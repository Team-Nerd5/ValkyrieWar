// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GachaRandomDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FGachaRandomDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 GroupId = 0;
	UPROPERTY(EditAnywhere)
	int32 Rate = 0;
	UPROPERTY(EditAnywhere)
	int32 ResultGrade = 0;
	UPROPERTY(EditAnywhere)
	TArray<int32> TargetValkyires;
};
