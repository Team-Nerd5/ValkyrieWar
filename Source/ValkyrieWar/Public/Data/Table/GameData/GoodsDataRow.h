// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "GoodsDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FGoodsDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	EGoodsType Type = EGoodsType::Gold;
	UPROPERTY(EditAnywhere)
	FString Name = TEXT("");
	UPROPERTY(EditAnywhere)
	int32 Order = 0;
};
