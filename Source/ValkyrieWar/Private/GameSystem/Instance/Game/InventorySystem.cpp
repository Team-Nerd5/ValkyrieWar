// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Module/ItemModule.h"

void UInventorySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency(UDataManager::StaticClass());

	DataManager = Cast<UDataManager>(GetGameInstance()->GetSubsystem<UDataManager>());

	GetFilteredInventoryList(EItemGroup::None);
}

TArray<UItemData*> UInventorySystem::GetFilteredInventoryList(EItemGroup InItemGroup)
{
	TArray<UItemData*> FilteredResult;

#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		FilteredResult.Empty();
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(Filter)] ItemModule이 없습니다"));
		return FilteredResult;
	}
#pragma endregion
	
	if (InItemGroup == EItemGroup::None)
	{
		// 인벤토리 전체 불러오기
		for (auto& Data : DataManager->GetItemModule()->GetItems())
		{
			UItemData* Item = Data.Value;
			FilteredResult.Add(Item);
		}
	}
	else
	{
		// 특정 아이템그룹만 불러오기
		for (auto& Data : DataManager->GetItemModule()->GetItems())
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
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(AddItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	UItemData* Item = nullptr;
	if (DataManager->GetItemModule()->GetItems().Contains(InUID))
	{
		Item = DataManager->GetItemModule()->GetItem(InUID);
	}

	// UID로 찾았을 경우
	if (Item)
	{
		// 아이템이 있을 때
		if (Item->GetItemGroup() == EItemGroup::None ||
			Item->GetItemGroup() == EItemGroup::Growth)
		{
			// 장비가 아닐 때
			DataManager->GetItemModule()->AddItemAmount(Item->GetUID(), InAmount);
		}
		else
		{
			// 장비일 때
			DataManager->GetItemModule()->AddItem(Item->GetTableData()->DataId, InAmount);
		}
		return;
	}

	// UID로 못찾았을 경우
	if (!Item)
	{
		UItemData* Data = nullptr;
		for (auto& Pair : DataManager->GetItemModule()->GetItems())
		{
			UItemData* PairItem = Pair.Value;
			if (PairItem && PairItem->GetTableData() && PairItem->GetTableData()->DataId == InDataId)
			{
				Data = PairItem;
				break;
			}
		}

		// DataID로 찾았을 경우
		if (Data)
		{
			// 아이템이 있을 때
			if (Data->GetItemGroup() == EItemGroup::None ||
				Data->GetItemGroup() == EItemGroup::Growth)
			{
				// 장비가 아닐 때
				DataManager->GetItemModule()->AddItemAmount(Data->GetUID(), InAmount);
			}
			else
			{
				// 장비일 때
				for (int32 i = 0; i < InAmount; ++i)
				{
					DataManager->GetItemModule()->AddItem(InDataId, 1);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[InventorySystem(AddItem)] 아이템 새로 추가"));
			DataManager->GetItemModule()->AddItem(InDataId, 1);
		}
		return;
	}
}

void UInventorySystem::UseItem(UItemData* InItem, int32 InAmount)
{
#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UseItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion
	
	if (DataManager->GetItemModule()->GetItem(InItem->GetUID()))
	{
		DataManager->GetItemModule()->AddItemAmount(InItem->GetUID(), -InAmount);
	}
}

void UInventorySystem::SellItem(UItemData* InItem, int32 InAmount)
{
#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(SellItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	// 아이템 가격에 따른 플레이어 골드 추가 필요

	if (DataManager->GetItemModule()->GetItem(InItem->GetUID()))
	{
		DataManager->GetItemModule()->AddItemAmount(InItem->GetUID(), -InAmount);
	}
}

void UInventorySystem::EquipItem(UItemData* InItem, int64 InCharacterUID)
{
#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(EquipItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	if (DataManager->GetItemModule()->GetItem(InItem->GetUID()))
	{
		InItem->Equip(InCharacterUID);
	}
}

void UInventorySystem::UnEquipItem(UItemData* InItem)
{
#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UnEquipItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	if (DataManager->GetItemModule()->GetItem(InItem->GetUID()))
	{
		InItem->Equip(0);
	}
}

void UInventorySystem::TestAddItem()
{
	AddItem(1000000001, 1, 1);
	AddItem(1000000002, 2, 1);
	AddItem(1000000003, 3, 1);
	AddItem(1000000004, 4, 1);
	AddItem(1000000005, 5, 1);
	AddItem(1000000006, 6, 1);
	AddItem(1000000007, 7, 1);
	AddItem(1000000008, 8, 1);
	AddItem(1000000009, 9, 1);
	AddItem(1000000010, 10, 1);

	UWorldEventSystem* WorldEvent = GetWorld()->GetSubsystem<UWorldEventSystem>();
	WorldEvent->Widget.OnChangeItemAmount.Broadcast();
}
