// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleRewardItemWIdget.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/ItemModule.h"

void UBattleRewardItemWIdget::SetRewardItem(int32 InItemDataId, int32 InAmount)
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UItemModule* ItemModule = DataManager->GetItemModule();
	if (!ItemModule)
		return;

	UTexture2D* Icon = ItemModule->GetTableDataById(InItemDataId).Icon.LoadSynchronous();
	if (Icon)
	{
		if (ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(Icon);
		}
	}

	if (RewardAmountBox)
		RewardAmountBox->SetVisibility(InAmount > 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if (RewardAmount)
	{
		if (InAmount > 1)
		{
			RewardAmount->SetText(FText::AsNumber(InAmount));
		}
	}
}
