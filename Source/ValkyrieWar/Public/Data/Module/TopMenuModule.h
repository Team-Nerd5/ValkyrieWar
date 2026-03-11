// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/TopMenuDataRow.h"
#include "TopMenuModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTopMenuModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FTopMenuDataRow GetTopMenuTable(int32 InKey);
protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FTopMenuDataRow> TableDataByDataId;
};
