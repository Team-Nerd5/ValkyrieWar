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

	//현우 : 원래 하고자 했던 방식..
	FORCEINLINE void SetCurrentStage(int32 InDataId)
	{
		//전투 시작하면 저장
		CurrentStageDataId = InDataId;
	}

	FORCEINLINE void SetNextStage()
	{
		NextOpenStageDataId = GetNextStageId();
	}
	FORCEINLINE int32 GetNextStageDataId()
	{
		return NextOpenStageDataId;
	}

	FORCEINLINE int32 GetCurrentStage() const { return CurrentStageDataId; }

	FORCEINLINE int32 GetCurrentRewardGroupId() const
	{
		if (TableDataByDataId.Contains(CurrentStageDataId))
			return TableDataByDataId.FindChecked(CurrentStageDataId).RewardGroupId;
		else
			return 0;
	}

	FORCEINLINE FStageInfoDataRow GetInfoData(int32 InDataId)
	{
		if (TableDataByDataId.Contains(InDataId))
			return TableDataByDataId.FindChecked(InDataId);
		else
			return FStageInfoDataRow();
	}

protected:
	virtual void MakeData() override;

	int32 GetNextStageId();
private:
	static int64 MakeChapterStageKey(int32 InChapter, int32 InStageNum);

private:
	UPROPERTY()
	TMap<int32, FStageInfoDataRow> TableDataByDataId;

	UPROPERTY()
	TMap<int64, FStageInfoDataRow> TableDataByChapterStageKey;

	//현재 캐싱된 스테이지 데이터 아이디.
	UPROPERTY()
	int32 CurrentStageDataId = 0;

	//다음 스테이지로 열어줄 데이터 id. 있으면 로비에서 열어줌
	UPROPERTY()
	int32 NextOpenStageDataId = 0;
};
