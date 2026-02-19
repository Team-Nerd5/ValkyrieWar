// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ItemSaveGame.generated.h"

/**
 * 
 */

struct ItemDataStruct
{
	uint64 UID;
	int32 Amount;
	uint64 EquipCharacter;
};

UCLASS()
class VALKYRIEWAR_API UItemSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	TMap<uint64, ItemDataStruct*> ItemDataList;
};
