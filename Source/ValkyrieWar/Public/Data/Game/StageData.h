// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/StageDataRow.h"
#include "Data/Table/GameData/StageInfoDataRow.h"

#include "GameSystem/Instance/Game/GameManager.h"

#include "StageData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UStageData : public UObject
{
	GENERATED_BODY()

public:
	void MakeData(const FStageDataRow InTableData, UGameManager* InGameManager);

	FORCEINLINE int32 GetChapter() { return TableData.Chapter; }
protected:
	UPROPERTY()
	FStageDataRow TableData;

	UPROPERTY()
	TMap<int32, FStageInfoDataRow> StageInfoData;
};
