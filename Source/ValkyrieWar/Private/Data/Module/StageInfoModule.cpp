#include "Data/Module/StageInfoModule.h"

int64 UStageInfoModule::MakeChapterStageKey(int32 InChapter, int32 InStageNum)
{
	return (static_cast<int64>(InChapter) << 32) | static_cast<uint32>(InStageNum);
}

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

TMap<int32, FStageInfoDataRow> UStageInfoModule::GetChapterStageInfo(int32 InChapter) const
{
	TMap<int32, FStageInfoDataRow> OutData;

	for (const TPair<int32, FStageInfoDataRow>& Pair : TableDataByDataId)
	{
		if (Pair.Value.ChapterNum == InChapter)
		{
			OutData.Add(Pair.Value.StageNum, Pair.Value);
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

	const FStageInfoDataRow* Found = TableDataByChapterStageKey.Find(MakeChapterStageKey(InChapter, InStageNum));
	if (!Found)
	{
		return false;
	}

	OutRow = *Found;
	return true;
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

bool UStageInfoModule::GetRewardGroupIdByChapterAndStage(int32 InChapter, int32 InStageNum, int32& OutRewardGroupId) const
{
	OutRewardGroupId = 0;

	FStageInfoDataRow Row;
	if (!GetStageInfoByChapterAndStage(InChapter, InStageNum, Row))
	{
		return false;
	}

	OutRewardGroupId = Row.RewardGroupId;
	return OutRewardGroupId > 0;
}

int32 UStageInfoModule::GetNextStageId()
{
	if (CurrentStageDataId <= 0)
		return 0;

	if (TableDataByDataId.Contains(CurrentStageDataId))
	{
		return TableDataByDataId.FindChecked(CurrentStageDataId).NextStageDataId;
	}

	return 0;
}

void UStageInfoModule::MakeData()
{
	TableDataByDataId.Empty();
	TableDataByChapterStageKey.Empty();

	if (!DataTable)
	{
		return;
	}

	TArray<FStageInfoDataRow*> AllRows;
	DataTable->GetAllRows<FStageInfoDataRow>(TEXT("StageInfoModule_Init"), AllRows);

	for (FStageInfoDataRow* Item : AllRows)
	{
		if (!Item)
		{
			continue;
		}

		TableDataByDataId.Add(Item->DataId, *Item);
		TableDataByChapterStageKey.Add(MakeChapterStageKey(Item->ChapterNum, Item->StageNum), *Item);
	}
}
