// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
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

void UInventorySystem::AddItem(uint64 InUID, int32 InDataId, int32 InAmount)
{
#pragma region 유효성 검사
	if (!DataManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(AddItem)] DataManager가 없습니다"));
		return;
	}
	if (!(DataManager->GetItemModule()))
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(AddItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	// 테스트를 위해 UItemData가 아닌 ItemData의 UID와 DataID를 따로 받게 했음
	// 나중에 필요시 UItemData로 변경
		 
	UItemData* Item = DataManager->GetItemModule()->GetItem(InUID);

	// UID로 찾았을 때
	if (Item)	// 아이템이 있을 때
	{
		if (Item->GetItemGroup() == EItemGroup::Equip)	
		{
			for (int32 i = 0; i < InAmount; ++i)
			{
				DataManager->GetItemModule()->AddItem(Item->GetTableData().DataId, 1);			// 장비일 때
			}
		}
		else
		{
			DataManager->GetItemModule()->AddItemAmount(Item->GetUID(), InAmount);				// 장비가 아닐 때
		}
		return;
	}

	//아래는 왜 하는건지 모르겠음?
	// -> UID가 다르더라도 DataID가 같고 장비가 아닐 때 중첩하기 위한 코드(아이템류)
	
	// UID로 못찾았을 경우
	if (!Item)	
	{
		// 인벤토리에 DataId가 같은 아이템 찾기
		UItemData* ItemDataById = nullptr;
		for (UItemData* FoundItemData : DataManager->GetItemModule()->GetItems())
		{
			if (FoundItemData && FoundItemData->GetTableData().DataId == InDataId)
			{
				ItemDataById = FoundItemData;
				break;
			}
		}

		if (ItemDataById)	// DataID로 찾았을 경우
		{
			if (ItemDataById->GetItemGroup() == EItemGroup::Equip)
			{
				for (int32 i = 0; i < InAmount; ++i)									
				{
					DataManager->GetItemModule()->AddItem(InDataId, 1);							// 장비일 때
				}
			}
			else
			{
				DataManager->GetItemModule()->AddItemAmount(ItemDataById->GetUID(), InAmount);	// 장비가 아닐 때
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[InventorySystem(AddItem)] 아이템 새로 추가"));		// 아이템을 새로 얻었을 때
			DataManager->GetItemModule()->AddItem(InDataId, InAmount);
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
	if (!InItem)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UseItem)] Item이 없습니다"));
		return;
	}
#pragma endregion

	DataManager->GetItemModule()->AddItemAmount(InItem->GetUID(), -InAmount);
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

	DataManager->GetItemModule()->AddItemAmount(InItem->GetUID(), -InAmount);
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
	WorldEvent->Widget.OnUpdateInventory.Broadcast();
	WorldEvent->Widget.OnUpdateInventoryAmountChanged.Broadcast();
}
