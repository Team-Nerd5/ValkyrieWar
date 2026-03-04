// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/CharacterInfoWidget.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "Widget/HUD/LobbyWidget.h"

void UCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UCharacterInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UCharacterInfoWidget::OpenUI()
{
	UIType = EUIType::PopupCharacterInfo;

	Super::OpenUI();
}

void UCharacterInfoWidget::CloseUI()
{
	//로비 켜줘야함
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->OpenUI<ULobbyWidget>(EUIType::Lobby);
	}
	Super::CloseUI();
}
