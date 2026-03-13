// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/BattlePauseMenuButtonWidget.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "Widget/Popup/Battle/BattlePauseMenuWidget.h"

void UBattlePauseMenuButtonWidget::NativeConstruct()
{
	if (Btn_Menu)
	{
		Btn_Menu->OnClicked.AddUniqueDynamic(this, &UBattlePauseMenuButtonWidget::HandleMenuClicked);
	}
}

void UBattlePauseMenuButtonWidget::NativeDestruct()
{
	if (Btn_Menu)
	{
		Btn_Menu->OnClicked.RemoveDynamic(this, &UBattlePauseMenuButtonWidget::HandleMenuClicked);
	}
}

void UBattlePauseMenuButtonWidget::HandleMenuClicked()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UBattlePauseMenuWidget* UattlePauseMenu = UIManager->OpenUI<UBattlePauseMenuWidget>(EUIType::PopupBattlePause);
	}
}
