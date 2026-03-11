// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ContentsModule.h"

void UContentsModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Contents);
		MakeData();
	}

	SendDataLoadComplete();
}

TArray<FContentsDataRow> UContentsModule::GetAllDataSorted()
{
	MenuContents.Sort([](const FContentsDataRow& A, const FContentsDataRow& B) {
		return A.Order < B.Order;
		});

	return MenuContents;
}

int32 UContentsModule::GetTopMenuId(EUIType InUIType)
{
	for (auto data : TableDataByDataId)
	{
		if (data.Value.MenuType == InUIType)
		{
			return data.Value.TopMenuId;
		}
	}

	return 0;
}

void UContentsModule::MakeData()
{
	if (DataTable)
	{
		TArray<FContentsDataRow*> AllRows;
		DataTable->GetAllRows<FContentsDataRow>(TEXT("ContentsModule_Init"), AllRows);

		for (FContentsDataRow* Item : AllRows)
		{
			if (!Item) continue;

			TableDataByDataId.Add(Item->DataId, *Item);

			if (Item->ContentType == EContentType::Menu)
			{
				MenuContents.Add(*Item);
			}
		}
	}
}

FContentsDataRow const UContentsModule::GetTableData(int32 inDataId)
{
	return *TableDataByDataId.Find(inDataId);
}
