// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/StageData.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UStageData::MakeData(const FStageDataRow InTableData, UGameManager* InGameManager)
{
	TableData = InTableData;

	if (InGameManager)
	{
		if (UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>())
		{
			StageInfoData = DataManager->GetStageInfoModule()->GetChapterStageInfo(GetChapter());
		}
	}
}
