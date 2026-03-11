// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "Data/Module/ItemModule.h"

void UInventorySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency(UDataManager::StaticClass());

	DataManager = GetGameInstance()->GetSubsystem<UDataManager>();

	// 인벤토리 데이터 테스트용
	//DataManager->CreateData();

	GetFilteredInventoryList(EItemGroup::None);
}

TArray<UItemData*> UInventorySystem::GetFilteredInventoryList(EItemGroup InItemGroup, EEquipGroup InEquipGroup)
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
		FilteredResult = DataManager->GetItemModule()->GetItems();
	}
	else
	{
		if (InEquipGroup == EEquipGroup::None)
		{
			// 특정 아이템 그룹 전부 불러오기
			for (UItemData* Item : DataManager->GetItemModule()->GetItems())
			{
				if (Item && Item->GetItemGroup() == InItemGroup)
				{
					FilteredResult.Add(Item);
				}
			}
		}
		else
		{
			// 특정 아이템 그룹 내 특정 장비 그룹 불러오기
			for (UItemData* Item : DataManager->GetItemModule()->GetItems())
			{
				if (Item && Item->GetItemGroup() == InItemGroup && Item->GetEquipGroup() == InEquipGroup)
				{
					FilteredResult.Add(Item);
				}
			}
		}
	}
	return FilteredResult;
}

UItemData* UInventorySystem::GetEquippedItemByGroup(uint64 InCharacterUID, EEquipGroup InItemGroup)
{
#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(Equipped)] ItemModule이 없습니다"));
		return nullptr;
	}
#pragma endregion

	for (UItemData* Item : DataManager->GetItemModule()->GetItems())
	{
		if (InCharacterUID == 0)
		{
			return nullptr;
		}

		if (Item &&
			Item->GetEquipCharacter() == InCharacterUID &&
			Item->GetItemGroup() == EItemGroup::Equip &&
			Item->GetEquipGroup() == InItemGroup)
		{
			return Item;
		}
	}
	return nullptr;
}

TArray<UItemData*> UInventorySystem::GetAllItems()
{
	TArray<UItemData*> Items;

	if (UItemModule* ItemData = DataManager->GetItemModule())
	{
		TArray<UItemData*> AllItem = ItemData->GetItems();

		for (UItemData* Data : AllItem)
		{
			if (Data->GetEquipCharacter() > 0)
				continue;

			Items.Add(Data);
		}
	}

	return Items;
}

TArray<UItemData*> UInventorySystem::GetEquipItems()
{
	TArray<UItemData*> EquipItems;

	if (UItemModule* ItemData = DataManager->GetItemModule())
	{
		TArray<UItemData*> Items = ItemData->GetItems();

		for(UItemData* Item : Items)
		{
			if (Item && Item->GetItemGroup() == EItemGroup::Equip)
			{
				EquipItems.Add(Item);
			}
		}
	}
	return EquipItems;
}

void UInventorySystem::AddItem(int32 InDataId, int32 InAmount)
{
	if (!DataManager)
	{
		DataManager = GetGameInstance()->GetSubsystem<UDataManager>();
	}

	DataManager->GetItemModule()->AddItem(InDataId, InAmount);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
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
	if (!InItem)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UseItem)] Item이 없습니다"));
		return;
	}
#pragma endregion

	bool ItemExists = DataManager->GetItemModule()->AddItemAmount(InItem->GetUID(), -InAmount);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		if(ItemExists)
			WorldEventSystem->Widget.OnInventoryItemAmountChanged.Broadcast(InItem->GetUID());
		else
			WorldEventSystem->Widget.OnUpdateInventory.Broadcast();

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
	if (!InItem)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(SellItem)] Item이 없습니다"));
		return;
	}
#pragma endregion

	int32 Price = InItem->GetTableData().SellPrice;

	bool ItemExists = DataManager->GetItemModule()->AddItemAmount(InItem->GetUID(), -InAmount);

	if (USaveManager* Save = GetGameInstance()->GetSubsystem<USaveManager>())
	{
		Save->AddGoods(EGoodsType::Gold, Price * InAmount);
	}

	//인벤토리 업데이트
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		if (ItemExists)
			WorldEventSystem->Widget.OnInventoryItemAmountChanged.Broadcast(InItem->GetUID());
		else
			WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	}
}

void UInventorySystem::EquipItem(UItemData* InItem, uint64 InCharacterUID)
{
#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(EquipItem)] ItemModule이 없습니다"));
		return;
	}
	if (!InItem)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(EquipItem)] InItem이 없습니다"));
		return;
	}
	if (InItem->GetItemGroup() != EItemGroup::Equip)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(EquipItem)] 장착할 수 없는 아이템 입니다"));
		return;

	}
#pragma endregion

	// 대상 캐릭터가 같은 부위에 이미 장착한 아이템이 있다면 장착해제
	if (InItem->GetEquipGroup() != EEquipGroup::None)
	{
		if (UItemData* AlreadyEquipped = GetEquippedItemByGroup(InCharacterUID, InItem->GetEquipGroup()))
		{
			AlreadyEquipped->Equip(0);
		}
	}
	// 장착할 아이템이 대상 캐릭터가 아닌 이미 다른 캐릭터에 장착되어있다면 장착 해제
	uint64 ItemOwner = InItem->GetEquipCharacter();
	if (ItemOwner != 0 && ItemOwner != InCharacterUID)
	{
		InItem->Equip(0);
	}

	// 최종적으로 InItem을 대상 케릭터에 장착
	InItem->Equip(InCharacterUID);
}

void UInventorySystem::UnEquipItem(UItemData* InItem)
{
#pragma region 유효성 검사
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UnEquipItem)] ItemModule이 없습니다"));
		return;
	}
	if (!InItem)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UnEquipItem)] InItem이 없습니다"));
		return;
	}
#pragma endregion

	InItem->Equip(0);
}
