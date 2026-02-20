// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ValkyrieModule.h"

void UValkyrieModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Valkyrie);
		MakeData();
	}
	SendDataLoadComplete();
}

void UValkyrieModule::MakeData()
{
	if (DataTable)
	{
		TArray<FValkyrieDataRow*> AllRows;
		DataTable->GetAllRows<FValkyrieDataRow>(TEXT("ValkyrieModule_Init"), AllRows);

		for (FValkyrieDataRow* Valkyrie : AllRows)
		{
			TableDataByDataId.Add(Valkyrie->DataId, Valkyrie);
		}
	}
}
