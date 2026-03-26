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

void ULobbyMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (MenuButton)
	{
		MenuButton->OnClicked.RemoveDynamic(this, &ULobbyMenuWidget::OnClickMenu);
	}
}

void ULobbyMenuWidget::SetData(FContentsDataRow InTableData)
{
	if (!InTableData.Icon.IsNull())
	{
		UTexture2D* IconTexture = InTableData.Icon.LoadSynchronous();

		FButtonStyle NewButtonStyle = MenuButton->GetStyle();

		NewButtonStyle.Normal.SetResourceObject(IconTexture);
		NewButtonStyle.Hovered.SetResourceObject(IconTexture);
		NewButtonStyle.Pressed.SetResourceObject(IconTexture);

		MenuButton->SetStyle(NewButtonStyle);
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
