// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/StateEnums.h"
#include "BattleEventData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleStateChanged, EBattleState, InState);
/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FBattleEventData
{
public:
	GENERATED_BODY()
public:
	FOnBattleStateChanged OnBattleStateChanged;
};
