// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/StateEnums.h"
#include "BattleEventData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleStateChanged, EBattleState, InState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAllyWallHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyWallHealthChanged, float, CurrentHealth, float, MaxHealth);

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
	FOnAllyWallHealthChanged OnAllyWallHealthChanged;
	FOnEnemyWallHealthChanged OnEnemyWallHealthChanged;
};
