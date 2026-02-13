// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/InventorySystem.h"

#include "GameSystem/Instance/Game/DataManager.h"

void UInventorySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency(UDataManager::StaticClass());

	DataManager = Cast<UDataManager>(GetGameInstance()->GetSubsystem<UDataManager>());

	ItemModule = NewObject<UItemModule>(this, UItemModule::StaticClass());
}

TArray<UItemData*> UInventorySystem::GetFilteredInventoryList(EItemGroup InItemGroup)
{
	TArray<UItemData*> FilteredResult;

#pragma region 유효성 검사
	if (!ItemModule)
	{
		FilteredResult.Empty();
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(Filter)] ItemModule이 없습니다"));
		return FilteredResult;
	}
#pragma endregion
	
	if (InItemGroup == EItemGroup::None)
	{
		// 인벤토리 전체 불러오기
		for (auto& Data : ItemModule->GetItems())
		{
			UItemData* Item = Data.Value;
			FilteredResult.Add(Item);
		}
	}
	else
	{
		// 특정 아이템그룹만 불러오기
		for (auto& Data : ItemModule->GetItems())
		{
			UItemData* Item = Data.Value;
			if (Item && Item->GetItemGroup() == InItemGroup)
			{
				FilteredResult.Add(Item);
			}
		}
	}
	return FilteredResult;
}

void UInventorySystem::AddItem(uint64 InUID, int32 InDataId, int32 InAmount)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(AddItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	UItemData* Item = nullptr;
	if (ItemModule->GetItems().Contains(InUID))
	{
		Item = ItemModule->GetItem(InUID);
	}

	if (Item == nullptr)
	{
		// 아이템이 없을 때
		ItemModule->AddItem(InDataId, InAmount);
		return;
	}

	// 아이템이 있을 때
	if (Item->GetItemGroup() == EItemGroup::None ||
		Item->GetItemGroup() == EItemGroup::Growth)
	{
		// 장비가 아닐 때
		ItemModule->AddItemAmount(Item->GetUID(), InAmount);
	}
	else
	{
		// 장비일 때
		ItemModule->AddItem(Item->GetTableData()->DataId, InAmount);
	}
	return;
}

void UInventorySystem::UseItem(UItemData* InItem, int32 InAmount)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UseItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion
	
	if (ItemModule->GetItem(InItem->GetUID()))
	{
		ItemModule->AddItemAmount(InItem->GetUID(), -InAmount);
	}
}

void UInventorySystem::SellItem(UItemData* InItem, int32 InAmount)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(SellItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	// 아이템 가격에 따른 플레이어 골드 추가 필요

	if (ItemModule->GetItem(InItem->GetUID()))
	{
		ItemModule->AddItemAmount(InItem->GetUID(), -InAmount);
	}
}

void UInventorySystem::EquipItem(UItemData* InItem, int64 InCharacterUID)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(EquipItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	if (ItemModule->GetItem(InItem->GetUID()))
	{
		InItem->SetEquipCharacterUID(InCharacterUID);
	}
}

void UInventorySystem::UnEquipItem(UItemData* InItem)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UnEquipItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	if (ItemModule->GetItem(InItem->GetUID()))
	{
		InItem->SetEquipCharacterUID(0);
	}
}

void UInventorySystem::TestAddItem()
{
	AddItem(1, 1, 1);
	AddItem(1, 2, 1);
	AddItem(1, 3, 1);
	AddItem(1, 4, 1);
	AddItem(1, 5, 1);
	AddItem(1, 6, 1);
	AddItem(1, 7, 1);
}
