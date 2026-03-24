// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Enum/DataEnums.h"
#include "GoodsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGoodsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	FORCEINLINE uint64 GetGoods(EGoodsType InGoodsType)
	{
		if (Goods.Contains(InGoodsType))
		{
			return Goods.FindChecked(InGoodsType);
		}

		return 0;
	}
	FORCEINLINE void AddGoods(EGoodsType InGoodsType, int64 InAmount)
	{
		Goods.FindOrAdd(InGoodsType) += InAmount;
	}

protected:
	UPROPERTY()
	TMap<EGoodsType, uint64> Goods;
};
