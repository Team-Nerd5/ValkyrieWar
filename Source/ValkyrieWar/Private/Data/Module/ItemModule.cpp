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
}

void UItemModule::MakeData()
{
	if (DataTable)
	{
		TArray<FItemDataRow*> AllRows;
		DataTable->GetAllRows<FItemDataRow>(TEXT("ItemManager_Init"), AllRows);

		for (FItemDataRow* Item : AllRows)
		{
			TableDataByDataId.Add(Item->DataId, Item);
		}
	}
}

void UItemModule::AddItem(int32 InDataId, int32 Amount)
{
	FItemDataRow* TableData = GetTableDataById(InDataId);

	if (TableData)
	{
		UItemData* NewItem = NewObject<UItemData>(this);

		NewItem->Initialize(TableData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Item Table Data is not Exist!"));
	}
}

FItemDataRow* UItemModule::GetTableDataById(int32 InDataId)
{
	return *TableDataByDataId.Find(InDataId);
}
