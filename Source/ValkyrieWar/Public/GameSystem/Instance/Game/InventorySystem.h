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
	TArray<UItemData*> GetFilteredInventoryList(EItemGroup InItemGroup, EEquipGroup InEquipGroup = EEquipGroup::None);

	UItemData* GetEquippedItemByGroup(uint64 InCharacterUID, EEquipGroup InItemGroup);

	TArray<UItemData*> GetAllItems();
	TArray<UItemData*> GetEquipItems();

	//이게 원래 맞음..UID로 호출 하면 안됨
	void AddItem(int32 InDataId, int32 InAmount);

	UFUNCTION(BlueprintCallable)
	void UseItem(UItemData* InItem, int32 InAmount);

	UFUNCTION(BlueprintCallable)
	void SellItem(UItemData* InItem, int32 InAmount);

	void EquipItem(UItemData* InItem, uint64 InCharacterUID);

	UFUNCTION(BlueprintCallable)
	void UnEquipItem(UItemData* InItem);

private:
	UPROPERTY()
	TObjectPtr<class UDataManager> DataManager;

// 테스트용(삭제 가능)
public:
	// 아이템 추가 테스트용 함수
	UFUNCTION(BlueprintCallable)
	void TestAddItem();
};
