// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Enums.h"
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
	E_UITYPE UIType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UBaseWidget> WidgetClass;
};
