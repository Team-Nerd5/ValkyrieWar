// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Game/ValkyrieData.h"
#include "ModuleEventData.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValkyrieGenerated, int64, InUID, UValkyrieData*, InData);
/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FModuleEventData
{
	GENERATED_BODY()
public:
	//FOnValkyrieGenerated OnValkyrieGenerated;
};
