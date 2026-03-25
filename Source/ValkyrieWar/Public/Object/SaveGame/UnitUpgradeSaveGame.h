// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Enum/DataEnums.h"
#include "UnitUpgradeSaveGame.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FUnitDataStruct
{
	GENERATED_BODY()
public:
	uint64 UID;
	EUnitCharacterType UnitType;
	int32 Level;
	EGradeType Grade;
};

UCLASS()
class VALKYRIEWAR_API UUnitUpgradeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<uint64, FUnitDataStruct> OwnUnits;
};
