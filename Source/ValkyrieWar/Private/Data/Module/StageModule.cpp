// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/StageModule.h"
#include "Data/Table/GameData/StageDataRow.h"
#include "Data/Game/StageData.h"

void UStageModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Goods);
		MakeData();
	}

	SendDataLoadComplete();
}

void UStageModule::MakeData()
{
	if (DataTable)
	{
		TArray<FStageDataRow*> AllRows;
		DataTable->GetAllRows<FStageDataRow>(TEXT("StageModule_Init"), AllRows);

		for (FStageDataRow* Item : AllRows)
		{
			if (!Item) continue;

			UStageData* Stage = NewObject<UStageData>(this);
			Stage->MakeData(*Item, GameManager.Get());
			StageDataByChapter.Add(Item->Chapter, Stage);
		}
	}
}
