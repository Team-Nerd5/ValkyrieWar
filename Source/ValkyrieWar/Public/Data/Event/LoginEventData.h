// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums.h"
#include "LoginEventData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginStateChanged, ELoginState, InState);

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FLoginEventData
{
	GENERATED_BODY()
public:
	FOnLoginStateChanged OnLoginStateChanged;
};
