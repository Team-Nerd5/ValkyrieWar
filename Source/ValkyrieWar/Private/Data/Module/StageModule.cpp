// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/StageModule.h"
#include "Data/Table/GameData/StageDataRow.h"
#include "Data/Game/StageData.h"

void UStageModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Stage);
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

bool UStageModule::HasChapter(int32 InChapter) const
{
	return StageDataByChapter.Contains(InChapter);
}

void UStageModule::GetAllChapters(TArray<int32>& OutChapters) const
{
	OutChapters.Reset();
	StageDataByChapter.GetKeys(OutChapters);
	OutChapters.Sort();
}

int32 UStageModule::GetMinChapter() const
{
	TArray<int32> Chapters;
	GetAllChapters(Chapters);
	return Chapters.Num() > 0 ? Chapters[0] : 0;
}

int32 UStageModule::GetMaxChapter() const
{
	TArray<int32> Chapters;
	GetAllChapters(Chapters);
	return Chapters.Num() > 0 ? Chapters.Last() : 0;
}

void UStageModule::GetStageListViewDataByChapter(int32 InChapter, TArray<FStageEntryData>& OutStageList) const
{
	OutStageList.Reset();

	const TObjectPtr<UStageData>* FoundStageData = StageDataByChapter.Find(InChapter);
	if (!FoundStageData || !FoundStageData->Get())
	{
		return;
	}

	TArray<int32> StageNums;
	(*FoundStageData)->GetSortedStageNums(StageNums);

	for (const int32 StageNum : StageNums)
	{
		FStageEntryData Item;
		Item.Chapter = InChapter;
		Item.StageNum = StageNum;

		// TODO:
		// 나중에 세이브 데이터/해금 조건 연동
		Item.bLocked = false;
		Item.bCleared = false;

		OutStageList.Add(Item);
	}
}
