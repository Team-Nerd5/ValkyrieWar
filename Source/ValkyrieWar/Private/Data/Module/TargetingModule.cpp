// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/TargetingModule.h"

void UTargetingModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Targeting);
		MakeData();
	}

	SendDataLoadComplete();
}

void UTargetingModule::MakeData()
{
	if (DataTable)
	{
		TArray<FTargetingDataRow*> AllRows;
		DataTable->GetAllRows<FTargetingDataRow>(TEXT("TargetingModule_Init"), AllRows);

		for (FTargetingDataRow* Targetting : AllRows)
		{
			if (!Targetting) continue;

			TargetingData.Add(Targetting->DataId, *Targetting);
		}
	}
}
