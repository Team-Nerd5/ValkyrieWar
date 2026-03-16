// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
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
	EGradeType ResultGrade = EGradeType::Common;
	UPROPERTY(EditAnywhere)
	TArray<int32> TargetValkyires;
};
