// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/InventorySystem.h"

#include "GameSystem/Instance/Game/DataManager.h"

void UInventorySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


}

TArray<UItemData*> UInventorySystem::GetFilteredInventoryList(EItemGroup InItemGroup)
{
	TArray<UItemData*> FilteredResult;

	if (InItemGroup == EItemGroup::None)
	{
		FilteredResult = InventoryList;
		return FilteredResult;
	}

	for (UItemData* Item : InventoryList)
	{
		if (IsValid(Item) && Item->GetItemGroup() == InItemGroup)
		{
			FilteredResult.Add(Item);
		}
	}
	return FilteredResult;
}

void UInventorySystem::UseItem(UItemData* InItem)
{
}

void UInventorySystem::SellItem(UItemData* InItem, int32 InAmount)
{
}

void UInventorySystem::EquipItem(UItemData* InItem, int64 InCharacterUID)
{
}

void UInventorySystem::UnEquipItem(UItemData* InItem)
{
}
