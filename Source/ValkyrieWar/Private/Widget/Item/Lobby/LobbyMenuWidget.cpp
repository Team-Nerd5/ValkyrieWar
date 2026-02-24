// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Lobby/LobbyMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameSystem/Instance/Game/UIManager.h"

void ULobbyMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MenuButton)
	{
		MenuButton->OnClicked.AddDynamic(this, &ULobbyMenuWidget::OnClickMenu);
	}
}

void ULobbyMenuWidget::SetData(UTexture2D* Icon, FString Name, EUIType InOpenUIType)
{


	if (Icon)
	{
		//FButtonStyle Style;
		//FSlateBrush Brush;
		//Brush.
		//Style.SetNormal()
		//MenuButton->SetStyle()
	}

	if (MenuName)
	{
		MenuName->SetText(FText::FromString(Name));
	}

	ConnectedUIType = InOpenUIType;
	//연결 UIType...
}

void ULobbyMenuWidget::OnClickMenu()
{
	if (ConnectedUIType != EUIType::None)
	{
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			//이거 음..클래스 가져오는걸 음...
			UIManager->OpenUI<UBaseWidget>(ConnectedUIType);
		}
	}
}
