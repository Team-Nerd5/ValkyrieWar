// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleWinWidget.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UBattleWinWidget::SetReward(TArray<FRewardDataRow> InRewardList)
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UItemModule* ItemModule = DataManager->GetItemModule();
	if (!ItemModule)
		return;
	UGoodsModule* GoodsModule = DataManager->GetGoodsModule();
	if (!GoodsModule)
		return;

	RewardListBox->ClearChildren();
	for (const FRewardDataRow RewardData : InRewardList)
	{
		if (UBattleRewardBoxWidget* RewardWidget = CreateWidget<UBattleRewardBoxWidget>(GetWorld(), RewardBoxWidgetClass))
		{
			RewardWidget->SetRewardItem(RewardData);
			RewardListBox->AddChild(RewardWidget);
		}
	}
}
