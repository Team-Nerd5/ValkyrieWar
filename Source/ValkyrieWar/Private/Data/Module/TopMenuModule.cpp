// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/TopMenuModule.h"

void UTopMenuModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::TopMenu);
		MakeData();
	}

	SendDataLoadComplete();
}

FTopMenuDataRow UTopMenuModule::GetTopMenuTable(int32 InKey)
{
	return TableDataByDataId.FindChecked(InKey);
}

void UTopMenuModule::MakeData()
{
	if (DataTable)
	{
		TArray<FTopMenuDataRow*> AllRows;
		DataTable->GetAllRows<FTopMenuDataRow>(TEXT("TopMenuModule_Init"), AllRows);

		for (FTopMenuDataRow* Item : AllRows)
		{
			if (!Item) continue;

			TableDataByDataId.Add(Item->DataId, *Item);
		}
	}
}
