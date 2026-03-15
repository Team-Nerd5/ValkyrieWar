// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/StageInfoModule.h"

void UStageInfoModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::StageInfo);
		MakeData();
	}

	SendDataLoadComplete();
}

TMap<int32, FStageInfoDataRow> UStageInfoModule::GetChapterStageInfo(int32 InChapter)
{
	TMap<int32, FStageInfoDataRow> OutData;

	for (auto Data : TableDataByDataId)
	{
		if (Data.Value.ChapterNum == InChapter)
		{
			OutData.Add(Data.Value.StageNum, Data.Value);
		}
	}

	return OutData;
}

void UStageInfoModule::MakeData()
{
	if (DataTable)
	{
		TArray<FStageInfoDataRow*> AllRows;
		DataTable->GetAllRows<FStageInfoDataRow>(TEXT("StageModule_Init"), AllRows);

		for (FStageInfoDataRow* Item : AllRows)
		{
			if (!Item) continue;

			TableDataByDataId.Add(Item->DataId, *Item);
		}
	}
}
