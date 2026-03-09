// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Tools/Tab/TabMenuWidget.h"
#include "Widget/Item/Tools/Tab/TabMenuItemWidget.h"



#include "Components/PanelWidget.h"

void UTabMenuWidget::AddTab(ETabType InTabType, int32 InFilterIndex, FString InTabName)
{
	if (TabContainer)
	{
		UTabMenuItemWidget* TabItem = CreateWidget<UTabMenuItemWidget>(this, TabMenuItemClass);
		if (TabItem)
		{
			TabItem->SetTab(InTabType, InFilterIndex, InTabName);
			TabItem->OnTabItemSelected.AddDynamic(this, &UTabMenuWidget::HandleTabClicked);

			TabItems.Add(TabItem);

			TabContainer->AddChild(TabItem);
		}
	}
}

void UTabMenuWidget::SetTab(int32 InIndex)
{
	if (InIndex < TabItems.Num())
	{
		for (int32 i = 0; i < TabItems.Num(); i++)
		{			
			TabItems[i]->SetTabSelected(i == InIndex);
		}
	}
}

//탭 이미지 세팅
void UTabMenuWidget::HandleTabClicked(UTabMenuItemWidget* ClickedTab)
{
	for (UTabMenuItemWidget* TabItem : TabItems)
	{
		TabItem->SetTabSelected(TabItem == ClickedTab);
	}
}
