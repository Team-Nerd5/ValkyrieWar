// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UnitUpgradeSaveGame.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FUnitDataStruct
{
	GENERATED_BODY()
public:
	UPROPERTY()
	uint64 UID;
	UPROPERTY()
	int32 UnitType;
	UPROPERTY()
	int32 Level;
	UPROPERTY()
	int32 Grade;
};

UCLASS()
class VALKYRIEWAR_API UUnitUpgradeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<uint64, FUnitDataStruct> OwnUnits;
};
