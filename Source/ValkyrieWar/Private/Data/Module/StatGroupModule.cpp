// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/StatGroupModule.h"

void UStatGroupModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::StatGroup);
		MakeData();
	}

	SendDataLoadComplete();
}

void UStatGroupModule::MakeData()
{
	if (DataTable)
	{
		TArray<FStatGroupDataRow*> AllRows;
		DataTable->GetAllRows<FStatGroupDataRow>(TEXT("StatGroupModule_Init"), AllRows);

		for (FStatGroupDataRow* Item : AllRows)
		{
			TableDataByDataId.Add(Item->DataId, Item);
		}
	}
}
