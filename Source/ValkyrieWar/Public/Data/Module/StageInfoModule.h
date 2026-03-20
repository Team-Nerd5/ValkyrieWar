#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/StageInfoDataRow.h"
#include "StageInfoModule.generated.h"

UCLASS()
class VALKYRIEWAR_API UStageInfoModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

	TMap<int32, FStageInfoDataRow> GetChapterStageInfo(int32 InChapter) const;

	bool GetStageInfoByChapterAndStage(int32 InChapter, int32 InStageNum, FStageInfoDataRow& OutRow) const;
	bool GetEnemyUnitIdsByChapterAndStage(int32 InChapter, int32 InStageNum, TArray<int32>& OutUnitIds) const;
	bool GetEnemyLevelByChapterAndStage(int32 InChapter, int32 InStageNum, int32& OutEnemyLevel) const;
	bool GetRewardGroupIdByChapterAndStage(int32 InChapter, int32 InStageNum, int32& OutRewardGroupId) const;

protected:
	virtual void MakeData() override;

private:
	static int64 MakeChapterStageKey(int32 InChapter, int32 InStageNum);

private:
	UPROPERTY()
	TMap<int32, FStageInfoDataRow> TableDataByDataId;

	UPROPERTY()
	TMap<int64, FStageInfoDataRow> TableDataByChapterStageKey;
};
