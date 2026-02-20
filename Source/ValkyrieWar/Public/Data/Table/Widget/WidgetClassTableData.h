// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Enum/CommonEnums.h"
#include "GameSystem/Base/BaseWidget.h"
#include "WidgetClassTableData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FWidgetClassTableData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUIType UIType = EUIType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UBaseWidget> WidgetClass = nullptr;
};
