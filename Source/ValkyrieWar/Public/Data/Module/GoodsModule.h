// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/GoodsDataRow.h"
#include "GoodsModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGoodsModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;
protected:
	virtual void MakeData() override;

private:
	TMap<int32, FGoodsDataRow*> TableDataByDataId;
};
