// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/StageInfoDataRow.h"
#include "StageInfoModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UStageInfoModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	TMap<int32, FStageInfoDataRow> GetChapterStageInfo(int32 InChapter);

	bool GetStageInfoByChapterAndStage(int32 InChapter, int32 InStageNum, FStageInfoDataRow& OutRow) const;

protected:
	virtual void MakeData() override;
private:
	UPROPERTY()
	TMap<int32, FStageInfoDataRow> TableDataByDataId;
};
