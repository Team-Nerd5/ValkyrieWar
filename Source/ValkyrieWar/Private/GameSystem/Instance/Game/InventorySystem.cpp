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
			//장착된 장비는 인벤토리 미표기
			if (Item && Item->GetItemGroup() == EItemGroup::Equip && Item->GetEquipCharacter() == 0)
			{
				EquipItems.Add(Item);
			}
		}
	}
	return EquipItems;
}

UItemData* UInventorySystem::AddItem(int32 InDataId, int32 InAmount)
{
	if (!DataManager)
	{
		DataManager = GetGameInstance()->GetSubsystem<UDataManager>();
	}

	UItemData* AddedItem = DataManager->GetItemModule()->AddItem(InDataId, InAmount);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	}

	return AddedItem;
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

	UItemData* ChangedItem = DataManager->GetItemModule()->AddItem(InItem->GetTableData().DataId, -InAmount);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		if(ChangedItem)
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

	UItemData* ChangedItem = DataManager->GetItemModule()->SellItem(InItem->GetUID(), -InAmount);

	DataManager->GetGoodsModule()->Add(EGoodsType::Gold, Price * InAmount);

	//인벤토리 업데이트
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		if (ChangedItem)
			WorldEventSystem->Widget.OnInventoryItemAmountChanged.Broadcast(InItem->GetUID());
		else
			WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	}
}

//이걸 불러오려면...어차피 캐릭터를 로드했어야 함...인벤토리니까
void UInventorySystem::EquipItem(UItemData* InItem, UValkyrieData* InValkyrie)
{
	if (InValkyrie)
	{
		UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>();
		if (InValkyrie->IsEquipped(InItem->GetEquipGroup()))
		{
			InValkyrie->UnEquipItem(InItem->GetEquipGroup(), GameManager);
		}

		InItem->Equip(InValkyrie->GetUID());
		InValkyrie->EquipItem(InItem->GetEquipGroup(), InItem, GameManager);

		//아이템 저장...
		if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
		{
			SaveManager->AddSaveItem(InItem);
		}

		//인벤토리 아이템 개수가 바뀜
		if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
		}
	}
}

//캐릭터 정보 위젯의 장착 화면에서 장착 아이템을 해제했을 때
void UInventorySystem::UnEquipItem(UItemData* InItem, UValkyrieData* InValkyrie)
{
	if (InValkyrie)
	{
		UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>();
		if (InValkyrie->IsEquipped(InItem->GetEquipGroup()))
		{
			InValkyrie->UnEquipItem(InItem->GetEquipGroup(), GameManager);
		}				

		//인벤토리 아이템 개수가 바뀜
		if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
		}
	}
}
