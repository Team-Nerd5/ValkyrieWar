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

	FGoodsDataRow GetTableDataById(int32 InDataId);
	FGoodsDataRow GetTableData(EGoodsType InKey);

	void Update(EGoodsType InType, uint64 InAmount);
	void Add(EGoodsType InType, int64 InAmount);
	bool IsEnough(EGoodsType InType, uint64 InCheckAmount);

	FORCEINLINE uint64 Get(EGoodsType InType)
	{
		if (GoodsAmount.Contains(InType))
			return GoodsAmount.FindChecked(InType);
		else
		{
			GoodsAmount.Add(InType, 0);
			return 0;
		}
	}
protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FGoodsDataRow> TableDataById;
	UPROPERTY()
	TMap<EGoodsType, FGoodsDataRow> TableDataByType;

	UPROPERTY()
	TMap<EGoodsType, uint64> GoodsAmount;
};
