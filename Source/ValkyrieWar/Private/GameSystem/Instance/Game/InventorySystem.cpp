// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/InventorySystem.h"

#include "GameSystem/Instance/Game/DataManager.h"

void UInventorySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataManager = Cast<UDataManager>(GetGameInstance()->GetSubsystem<UDataManager>());
	ItemModule = DataManager->GetItemModule();
}

TArray<UItemData*> UInventorySystem::GetFilteredInventoryList(EItemGroup InItemGroup)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(Filter)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	TArray<UItemData*> FilteredResult;

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

	UItemData* Item = ItemModule->GetItem(InUID);
	if (!Item)
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
		ItemModule->AddItem(InDataId, InAmount);
	}	 
}

void UInventorySystem::UseItem(uint64 InUID, int32 InAmount)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UseItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	ItemModule->AddItemAmount(InUID, -InAmount);
}

void UInventorySystem::SellItem(uint64 InUID, int32 InAmount)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(SellItem)] ItemModule이 없습니다"));
		return;
	}
#pragma endregion

	// 아이템 가격에 따른 플레이어 골드 추가 필요

	ItemModule->AddItemAmount(InUID, -InAmount);
}

void UInventorySystem::EquipItem(uint64 InUID, int64 InCharacterUID)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(EquipItem)] ItemModule이 없습니다"));
		return;
	}
	UItemData* Item = ItemModule->GetItem(InUID);
	if (!Item)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(EquipItem)] 인벤토리에 장비가 없습니다 없습니다"));
		return;
	}
#pragma endregion

	Item->SetEquipCharacterUID(InUID);
}

void UInventorySystem::UnEquipItem(uint64 InUID)
{
#pragma region 유효성 검사
	if (!ItemModule)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UnEquipItem)] ItemModule이 없습니다"));
		return;
	}
	UItemData* Item = ItemModule->GetItem(InUID);
	if (!Item)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventorySystem(UnEquipItem)] 인벤토리에 장비가 없습니다"));
		return;
	}
#pragma endregion

	Item->SetEquipCharacterUID(0);
}
