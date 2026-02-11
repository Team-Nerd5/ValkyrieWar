// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Data/Module/ItemModule.h"
#include "Data/Enums.h"
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
	UFUNCTION(BlueprintCallable)
	TArray<UItemData*> GetFilteredInventoryList(EItemGroup InItemGroup);

	UFUNCTION(BlueprintCallable)
	void UseItem(UItemData* InItem);

	UFUNCTION(BlueprintCallable)
	void SellItem(UItemData* InItem, int32 InAmount);

	UFUNCTION(BlueprintCallable)
	void EquipItem(UItemData* InItem, int64 InCharacterUID);

	UFUNCTION(BlueprintCallable)
	void UnEquipItem(UItemData* InItem);

private:
	UItemModule* ItemModule;

	UWorldEventSystem* WorldEventSystem;

	TArray<UItemData*> InventoryList;
};
