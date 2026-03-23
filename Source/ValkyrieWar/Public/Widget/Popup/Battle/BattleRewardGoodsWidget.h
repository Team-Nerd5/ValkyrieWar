// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Data/Enum/DataEnums.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "BattleRewardGoodsWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleRewardGoodsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRewardGoods(EGoodsType InGoodsType, int32 InGoodsAmount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GoodsIcon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoodsAmount = nullptr;
};
