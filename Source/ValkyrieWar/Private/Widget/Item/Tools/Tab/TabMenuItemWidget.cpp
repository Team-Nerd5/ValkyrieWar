// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Tools/Tab/TabMenuItemWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"


void UTabMenuItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (TabButton)
	{
		TabButton->OnCheckStateChanged.AddDynamic(this, &UTabMenuItemWidget::OnTabButtonClicked);
	}

	SetTabSelected(bIsOn);
}

void UTabMenuItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (TabButton)
	{
		TabButton->OnCheckStateChanged.RemoveDynamic(this, &UTabMenuItemWidget::OnTabButtonClicked);
	}
}

void UTabMenuItemWidget::SetTabSelected(bool bSelected)
{
	if (TabButton)
	{
		TabButton->SetIsChecked(bSelected);
	}
}

void UTabMenuItemWidget::SetTab(ETabType InTabType, int32 InFilterIndex, FString InTabName)
{
	TabType = InTabType;

	if (TabName)
	{
		TabName->SetText(FText::FromString(InTabName));
	}

	FilterIndex = InFilterIndex;
}

void UTabMenuItemWidget::OnTabButtonClicked(bool bIsSelected)
{
	if (bIsSelected)
	{
		OnTabItemSelected.Broadcast(this);
		if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			EventSystem->Widget.OnTabMenuSelected.Broadcast(TabType, FilterIndex);
		}
	}
	else
	{
		if (TabButton)
		{
			TabButton->SetIsChecked(true);
		}
	}
}
