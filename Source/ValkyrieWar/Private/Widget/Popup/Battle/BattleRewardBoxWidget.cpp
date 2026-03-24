// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleRewardBoxWidget.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/GoodsModule.h"
#include "Data/Module/ItemModule.h"

void UBattleRewardBoxWidget::SetRewardItem(FRewardDataRow InRewardDataRow)
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UGoodsModule* GoodsModule = DataManager->GetGoodsModule();
	if (!GoodsModule)
		return;
	UItemModule* ItemModule = DataManager->GetItemModule();
	if (!ItemModule)
		return;

	if (InRewardDataRow.RewardType == ERewardType::Goods)
	{
		FGoodsDataRow GoodsData = GoodsModule->GetTableDataById(InRewardDataRow.RewardDataId);

		UTexture2D* RewardDataIcon = GoodsData.Icon.LoadSynchronous();
		if (RewardDataIcon)
		{
			if (RewardIcon)
			{
				RewardIcon->SetBrushFromTexture(RewardDataIcon);
			}
		}
		if (RewardAmount)
		{
			RewardAmount->SetText(FText::AsNumber(InRewardDataRow.Amount));
		}
	}
	else if (InRewardDataRow.RewardType == ERewardType::Item)
	{
		FItemDataRow ItemData = ItemModule->GetTableDataById(InRewardDataRow.RewardDataId);

		UTexture2D* RewardDataIcon = ItemData.Icon.LoadSynchronous();
		if (RewardDataIcon)
		{
			if (RewardIcon)
			{
				RewardIcon->SetBrushFromTexture(RewardDataIcon);
			}
		}
		if (RewardAmount)
		{
			RewardAmount->SetText(FText::AsNumber(InRewardDataRow.Amount));
		}
	}
}
