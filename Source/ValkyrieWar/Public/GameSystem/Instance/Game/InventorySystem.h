// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Data/Module/ItemModule.h"
#include "Data/Enums.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "InventorySystem.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UInventorySystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	TArray<UItemData*> GetFilteredInventoryList(EItemGroup InItemGroup);

	void AddItem(uint64 InUID, int32 InDataId, int32 InAmount);

	void UseItem(uint64 InUID, int32 InAmount);

	void SellItem(uint64 InUID, int32 InAmount);

	void EquipItem(uint64 InUID, int64 InCharacterUID);

	void UnEquipItem(uint64 InUID);

private:
	UDataManager* DataManager;
	UItemModule* ItemModule;
};
