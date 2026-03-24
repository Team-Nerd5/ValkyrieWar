// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleWinWidget.h"

void UBattleWinWidget::SetReward(TArray<FRewardDataRow> InRewardList)
{
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
