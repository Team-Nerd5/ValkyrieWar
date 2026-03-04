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

void ULobbyMenuWidget::SetData(FContentsDataRow InTableData)
{
	if (InTableData.Icon.IsValid())
	{
		UTexture2D* IconTexture = InTableData.Icon.LoadSynchronous();

		//FButtonStyle Style;
		//FSlateBrush Brush;
		//Brush.
		//Style.SetNormal()
		//MenuButton->SetStyle()
	}

	if (MenuName)
	{
		MenuName->SetText(FText::FromString(InTableData.Name));
	}

	CurrentMenuType = InTableData.MenuType;
}


void ULobbyMenuWidget::OnClickMenu()
{
	if (CurrentMenuType != EUIType::None)
	{
		OnMenuButtonClicked.Broadcast(CurrentMenuType);
	}
}
