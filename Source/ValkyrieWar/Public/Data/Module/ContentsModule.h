// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/ContentsDataRow.h"
#include "Data/Enum/DataEnums.h"
#include "ContentsModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UContentsModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	TArray<FContentsDataRow*> GetAllDataSorted();

	int32 GetGoodsId(EUIType InUIType);

protected:
	virtual void MakeData() override;

public:
	FContentsDataRow* const GetTableData(int32 InDataId);
private:
	TMap<int32, FContentsDataRow*> TableDataByDataId;
	TArray<FContentsDataRow*> MenuContents;
};
