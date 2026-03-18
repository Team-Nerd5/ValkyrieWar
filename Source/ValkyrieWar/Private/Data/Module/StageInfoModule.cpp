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

bool UStageInfoModule::GetStageInfoByChapterAndStage(int32 InChapter, int32 InStageNum, FStageInfoDataRow& OutRow) const
{
	if (InChapter <= 0 || InStageNum <= 0)
	{
		return false;
	}

	for (const TPair<int32, FStageInfoDataRow>& Pair : TableDataByDataId)
	{
		const FStageInfoDataRow& Row = Pair.Value;

		if (Row.ChapterNum == InChapter && Row.StageNum == InStageNum)
		{
			OutRow = Row;
			return true;
		}
	}

	return false;
}

bool UStageInfoModule::GetEnemyUnitIdsByChapterAndStage(int32 InChapter, int32 InStageNum, TArray<int32>& OutUnitIds) const
{
	OutUnitIds.Reset();

	FStageInfoDataRow Row;
	if (!GetStageInfoByChapterAndStage(InChapter, InStageNum, Row))
	{
		return false;
	}

	auto AddIfValid = [&OutUnitIds](int32 InUnitId)
		{
			if (InUnitId > 0)
			{
				OutUnitIds.Add(InUnitId);
			}
		};

	AddIfValid(Row.EnemyUnit1);
	AddIfValid(Row.EnemyUnit2);
	AddIfValid(Row.EnemyUnit3);
	AddIfValid(Row.EnemyUnit4);
	AddIfValid(Row.EnemyUnit5);

	return true;
}

bool UStageInfoModule::GetEnemyLevelByChapterAndStage(int32 InChapter, int32 InStageNum, int32& OutEnemyLevel) const
{
	OutEnemyLevel = 1;

	FStageInfoDataRow Row;
	if (!GetStageInfoByChapterAndStage(InChapter, InStageNum, Row))
	{
		return false;
	}

	OutEnemyLevel = FMath::Max(1, Row.EnemyLevel);
	return true;
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
