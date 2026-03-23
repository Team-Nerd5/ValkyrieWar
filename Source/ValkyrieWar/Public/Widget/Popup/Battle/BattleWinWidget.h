// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ScrollBox.h"

#include "Widget/Popup/Battle/BattleRewardGoodsWidget.h"
#include "Widget/Popup/Battle/BattleRewardItemWIdget.h"

#include "Data/Table/GameData/RewardDataRow.h"

#include "BattleWinWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleWinWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetReward(TMap<EGoodsType, int32> InRewardGoods, int32 InStageRewardGroupId);

	void RefreshUi();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleRewardGoodsWidget> Reward_Gold = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleRewardGoodsWidget> Reward_Gem = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleRewardGoodsWidget> Reward_Ticket = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> RewardListBox = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI|WidgetClass")
	TSubclassOf<UBattleRewardItemWIdget> RewardItemWidgetClass = nullptr;

	UPROPERTY()
	TArray<FRewardDataRow> RewardItems;
	UPROPERTY()
	TMap<EGoodsType, int32> RewardGoods;
};
