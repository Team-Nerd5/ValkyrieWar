// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Game/ItemData.h"
#include "ItemSaveGame.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FItemDataStruct
{
	GENERATED_BODY()
public:
	uint64 UID;
	int32 DataId;
	int32 Amount;
	uint64 EquipCharacter;
};

UCLASS()
class VALKYRIEWAR_API UItemSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TMap<uint64, FItemDataStruct> ItemDataList;

public:
	void AddItem(UItemData* InData);
	void RemoveItem(uint64 InUID);
};
