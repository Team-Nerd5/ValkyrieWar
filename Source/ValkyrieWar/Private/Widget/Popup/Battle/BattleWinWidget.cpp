// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleWinWidget.h"

void UBattleWinWidget::ClearReward()
{
	RewardListBox->ClearChildren();
}

void UBattleWinWidget::SetReward(FRewardDataRow InReward)
{
	if (UBattleRewardBoxWidget* RewardWidget = CreateWidget<UBattleRewardBoxWidget>(GetWorld(), RewardBoxWidgetClass))
	{
		RewardWidget->SetRewardItem(InReward);
		RewardListBox->AddChild(RewardWidget);
	}
}
