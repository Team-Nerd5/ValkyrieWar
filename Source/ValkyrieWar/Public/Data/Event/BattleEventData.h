// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/StateEnums.h"
#include "Data/Enum/CommonEnums.h"
#include "BattleEventData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleStateChanged, EBattleState, InState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAllyWallHealthChanged, float, InCurrentHealth, float, InMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyWallHealthChanged, float, InCurrentHealth, float, InMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInGameTimeChanged, float, InCurrentTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnUnitDataReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeClicked, int32, FamilyId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleModeChanged, EInputControlMode, InCurrentMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnUpgradeStateChanged,
	int32, FamilyId,
	int32, Level,
	int32, Cost,
	bool, bAffordable
);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSpawnLevelUpgraded, int32, FamilyId, int32, OldLevel, int32, NewLevel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaAdd, int32, InValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentManaChanged, int32, InCurrentMana);

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
	FOnInGameTimeChanged OnInGameTimeChanged;
	FOnBattleModeChanged OnBattleModeChanged;

	FOnSpawnUnitDataReady OnSpawnUnitDataReady;
	FOnUpgradeClicked OnUpgradeClicked;
	FOnUpgradeStateChanged OnUpgradeStateChanged;
	FOnSpawnLevelUpgraded OnSpawnLevelUpgraded;
	FOnManaAdd OnManaAdd;
	FOnCurrentManaChanged OnCurrentManaChanged;
};
