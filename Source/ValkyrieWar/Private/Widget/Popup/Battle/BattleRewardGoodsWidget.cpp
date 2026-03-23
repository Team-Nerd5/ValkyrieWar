// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleRewardGoodsWidget.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/GoodsModule.h"

void UBattleRewardGoodsWidget::SetRewardGoods(EGoodsType InGoodsType, int32 InGoodsAmount)
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UGoodsModule* GoodsModule = DataManager->GetGoodsModule();
	if (!GoodsModule)
		return;

	UTexture2D* Icon = GoodsModule->GetTableData(InGoodsType).Icon.LoadSynchronous();
	if (Icon)
	{
		if (GoodsIcon)
		{
			GoodsIcon->SetBrushFromTexture(Icon);
		}
	}
	if (GoodsAmount)
	{
		GoodsAmount->SetText(FText::AsNumber(InGoodsAmount));
	}
}
