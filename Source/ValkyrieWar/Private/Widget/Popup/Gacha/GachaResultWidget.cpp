// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Widget/Item/Gacha/GachaResultItemWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "Widget/Popup/Inventory/InventoryEntryWidget.h"

#include "Components/PanelWidget.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"

void UGachaResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NextCharacterButton)
		NextCharacterButton->OnClicked.AddDynamic(this, &UGachaResultWidget::OnClickNextCharacter);

	if (CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &UGachaResultWidget::OnClickCloseGacha);
}

void UGachaResultWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (NextCharacterButton)
		NextCharacterButton->OnClicked.RemoveDynamic(this, &UGachaResultWidget::OnClickNextCharacter);

	if (CloseButton)
		CloseButton->OnClicked.RemoveDynamic(this, &UGachaResultWidget::OnClickCloseGacha);
}

void UGachaResultWidget::OpenUI()
{
	UIType = EUIType::PopupGachaResult;

	Super::OpenUI();

	if (WrapBox_Result)
	{
		WrapBox_Result->SetVisibility(ESlateVisibility::Hidden);
	}

	if (RewardListPanel)
	{
		RewardListPanel->ClearChildren();
	}
}

void UGachaResultWidget::OnClickNextCharacter()
{
	if (EventSystem)
	{
		EventSystem->Lobby.OnShowNextGacha.Broadcast();
	}
}

void UGachaResultWidget::OnClickCloseGacha()
{
	if (EventSystem)
	{
		EventSystem->Lobby.OnLoadLobby.Broadcast();
	}
}

void UGachaResultWidget::ShowGachaResults()
{
	if (WrapBox_Result)	
		WrapBox_Result->SetVisibility(ESlateVisibility::Visible);

	if (RewardListPanel)
	{
		if (ResultData.Num() > 0)
		{
			for (UObject* Reward : ResultData)
			{
				if (ItemSpawnClass)
				{
					UInventoryEntryWidget* ItemWidget = CreateWidget<UInventoryEntryWidget>(this, ItemSpawnClass);
					if (UItemData* Item = Cast<UItemData>(Reward))
					{
						//아이템
						ItemWidget->SetData(Item);
					}
					else if (UValkyrieData* Valkyrie = Cast<UValkyrieData>(Reward))
					{
						//캐릭터
						ItemWidget->SetData(Valkyrie);
					}
					RewardListPanel->AddChild(ItemWidget);
				}				
			}
		}
	}

	if (NextCharacterButton)
		NextCharacterButton->SetVisibility(ESlateVisibility::Hidden);

}

void UGachaResultWidget::SetResultData(TArray<UObject*> InResultData)
{
	ResultData = InResultData;

	//결과 패널을 꺼줌
	if (WrapBox_Result)
		WrapBox_Result->SetVisibility(ESlateVisibility::Hidden);
		
	//스킵 버튼을 켜줌
	if (NextCharacterButton)
		NextCharacterButton->SetVisibility(ESlateVisibility::Visible);
	
}
