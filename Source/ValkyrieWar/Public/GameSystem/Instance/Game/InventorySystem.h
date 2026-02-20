// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Module/ItemModule.h"
#include "Data/Enum/DataEnums.h"

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

	void AddItem(uint64 InUID, int32 InDataId, int32 InAmount);

	UFUNCTION(BlueprintCallable)
	void UseItem(UItemData* InItem, int32 InAmount);

	UFUNCTION(BlueprintCallable)
	void SellItem(UItemData* InItem, int32 InAmount);

	void EquipItem(UItemData* InItem, uint64 InCharacterUID);

	UFUNCTION(BlueprintCallable)
	void UnEquipItem(UItemData* InItem);

	// 아이템 추가 테스트용 함수
	UFUNCTION(BlueprintCallable)
	void TestAddItem();

private:
	TObjectPtr<class UDataManager> DataManager;
};
