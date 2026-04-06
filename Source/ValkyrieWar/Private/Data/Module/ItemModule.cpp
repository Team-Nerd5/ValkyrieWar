// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ItemModule.h"

void UItemModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Item);
		MakeData();
	}

	SendDataLoadComplete();
}

void UItemModule::MakeData()
{
	//데이터 탐색 속도를 위해 Map으로 메모리에 올려둠
	if (DataTable)
	{
		TArray<FItemDataRow*> AllRows;
		DataTable->GetAllRows<FItemDataRow>(TEXT("ItemModule_Init"), AllRows);

		for (FItemDataRow* Item : AllRows)
		{
			if (!Item) continue;
			TableDataByDataId.Add(Item->DataId, *Item);
		}
	}
}

void UItemModule::SetList()
{
	OwnItemList.Empty();

	OwnItems.GenerateValueArray(OwnItemList);
}

uint64 UItemModule::GetExistItemUID(int32 InDataId)
{
	for(UItemData* Item : OwnItemList)
	{
		if (Item && Item->GetTableData().DataId == InDataId)
			return Item->GetUID();
	}

	return 0;
}

void UItemModule::LoadItem(uint64 InUID, int32 InDataId, int32 InAmount, uint64 InEquipCharacter)
{
	FItemDataRow TableData = GetTableDataById(InDataId);

	if (TableData.DataId > 0)
	{
		UItemData* NewItem = NewObject<UItemData>(this);

		NewItem->Initialize(InUID, InAmount, TableData, GameManager.Get());
		NewItem->Equip(InEquipCharacter);

		OwnItems.Add(InUID, NewItem);
		SetList();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%d Item Table Data is not Exist!"), InDataId);
	}
}

//아이템류는 중복저장하면 안됨(장비는 가능)
UItemData* UItemModule::AddItem(int32 InDataId, int32 InAmount)
{	
	uint64 ItemUID = GetExistItemUID(InDataId);
	if (ItemUID > 0)
	{
		UItemData* Item = GetItem(ItemUID);
		if (Item->GetItemGroup() == EItemGroup::Equip)
		{
			FItemDataRow TableData = GetTableDataById(InDataId);
			return AddNewItem(TableData, InAmount);
		}
		else
		{
			Item->AddAmount(InAmount);
			if (Item->GetAmount() <= 0)
			{
				OwnItems.Remove(ItemUID);

				if (USaveManager* SaveManager = GameManager->GetSubsystem<USaveManager>())
				{
					SaveManager->RemoveSaveItem(ItemUID);
				}

				SetList();

				return nullptr;
			}

			if (USaveManager* SaveManager = GameManager->GetSubsystem<USaveManager>())
			{
				SaveManager->AddSaveItem(Item);
			}

			return Item;
		}
	}
	else
	{
		FItemDataRow TableData = GetTableDataById(InDataId);
		return AddNewItem(TableData, InAmount);
	}
}

UItemData* UItemModule::SellItem(uint64 InUID, int32 InAmount)
{
	UItemData* Item = GetItem(InUID);

	Item->AddAmount(InAmount);
	if (Item->GetAmount() <= 0)
	{
		OwnItems.Remove(InUID);

		if (USaveManager* SaveManager = GameManager->GetSubsystem<USaveManager>())
		{
			SaveManager->RemoveSaveItem(InUID);
		}

		SetList();

		return nullptr;
	}

	if (USaveManager* SaveManager = GameManager->GetSubsystem<USaveManager>())
	{
		SaveManager->AddSaveItem(Item);
	}

	return Item;
}

UItemData* UItemModule::AddNewItem(FItemDataRow InTableData, int32 InAmount)
{
	if (InTableData.DataId > 0 && GameManager.IsValid())
	{
		UItemData* NewItem = NewObject<UItemData>(this);
		uint64 UID = GameManager->GetItemUID();

		NewItem->Initialize(UID, InAmount, InTableData, GameManager.Get());

		OwnItems.Add(UID, NewItem);
		SetList();

		if (USaveManager* SaveManager = GameManager->GetSubsystem<USaveManager>())
		{
			SaveManager->AddSaveItem(NewItem);
		}

		return NewItem;
	}

	return nullptr;
}

FItemDataRow UItemModule::GetTableDataById(int32 InDataId)
{
	if (TableDataByDataId.Contains(InDataId))
	{
		return *TableDataByDataId.Find(InDataId);
	}

	return FItemDataRow();
}

FItemDataRow UItemModule::GetMasteryItem(EWeaponType InWeaponType)
{
	for (auto Data : TableDataByDataId)
	{
		if (Data.Value.ItemType == EItemType::WeaponMasteryPiece && Data.Value.WeaponType == InWeaponType)
		{
			return Data.Value;
		}
	}
	return FItemDataRow();
}
