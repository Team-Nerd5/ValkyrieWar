// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleWinWidget.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Module/StageRewardModule.h"

void UBattleWinWidget::SetReward(TMap<EGoodsType, int32> InRewardGoods, int32 InStageRewardGroupId)
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UStageRewardModule* StageRewardModule = DataManager->GetStageRewardModule();
	if (!StageRewardModule)
		return;

	// 받은 Goods 저장
	RewardGoods = InRewardGoods;

	// 받은 Item 위젯 생성
	if (StageRewardModule->GetRewardRowsByStageRewardGroupId(InStageRewardGroupId, RewardItems))
	{
		RewardListBox->ClearChildren();
		for (FRewardDataRow& RewardRow : RewardItems)
		{
			if (UBattleRewardItemWIdget* RewardWidget = CreateWidget<UBattleRewardItemWIdget>(GetWorld(), RewardItemWidgetClass))
			{
				RewardWidget->SetRewardItem(RewardRow.DataId);
				RewardListBox->AddChild(RewardWidget);
			}
		}
	}

	// TODO: 얻은 재화 및 장비 업데이트 필요

	RefreshUi();
}

void UBattleWinWidget::RefreshUi()
{
	if (Reward_Gold)
	{
		int32 GoldAmount = RewardGoods.FindRef(EGoodsType::Gold);
		Reward_Gold->SetVisibility(GoldAmount ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		if (GoldAmount > 0)
			Reward_Gold->SetRewardGoods(EGoodsType::Gold, GoldAmount);
	}
	if (Reward_Gem)
	{
		int32 GemAmount = RewardGoods.FindRef(EGoodsType::Gem);
		Reward_Gem->SetVisibility(GemAmount ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		if (GemAmount > 0)
			Reward_Gem->SetRewardGoods(EGoodsType::Gem, GemAmount);
	}
	if (Reward_Ticket)
	{
		int32 TicketAmount = RewardGoods.FindRef(EGoodsType::Ticket);
		Reward_Ticket->SetVisibility(TicketAmount ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		if (TicketAmount > 0)
			Reward_Ticket->SetRewardGoods(EGoodsType::Ticket, TicketAmount);
	}
}
