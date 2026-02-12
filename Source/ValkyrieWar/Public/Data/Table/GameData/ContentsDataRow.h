// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/CommonEnums.h"
#include "ContentsDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FContentsDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId;
	UPROPERTY(EditAnywhere)
	FString Name;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere)
	EUIType OpenPopup;
	UPROPERTY(EditAnywhere)
	int32 Order;

};
