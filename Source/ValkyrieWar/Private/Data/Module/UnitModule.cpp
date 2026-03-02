// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/UnitModule.h"

void UUnitModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Unit);
		MakeData();
	}

	SendDataLoadComplete();
}

void UUnitModule::MakeData()
{
	if (DataTable)
	{
		TArray<FUnitDataRow*> AllRows;
		DataTable->GetAllRows<FUnitDataRow>(TEXT("UnitModule_Init"), AllRows);

		for (FUnitDataRow* Item : AllRows)
		{
			TableDataByDataId.Add(Item->DataId, *Item);

			UUnitData* UnitData = NewObject<UUnitData>(this);
			UnitData->MakeData(Item, GameManager.Get());

			OwnUnits.Add(Item->DataId, UnitData);
		}
	}
}
