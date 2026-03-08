// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/ItemListWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Widget/Item/Tools/Tab/TabMenuWidget.h"
#include "Components/PanelWidget.h"

void UItemListWidget::SetData(ETabType InTabType, TMap<int32, FString> InMenuNameData)
{
	TabType = InTabType;


	if (TabMenuClass)
	{
		UTabMenuWidget* TabMenu = CreateWidget<UTabMenuWidget>(this, TabMenuClass);

		if (TabMenu)
		{
			if (TabMenuContainer)
			{
				TabMenuContainer->AddChild(TabMenu);
			}

			for (auto Data : InMenuNameData)
			{
				int32 FilterIndex = Data.Key;
				FString TabName = Data.Value;
				TabMenu->AddTab(InTabType, FilterIndex, TabName);
			}
		}
	}
		
}

void UItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Widget.OnTabMenuSelected.AddDynamic(this, &UItemListWidget::OnTabMenuChanged);
	}
}

void UItemListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Widget.OnTabMenuSelected.RemoveDynamic(this, &UItemListWidget::OnTabMenuChanged);
	}
}

void UItemListWidget::OnTabMenuChanged(ETabType InTabType, int32 InSelectedTab)
{
	if(InTabType == TabType)
	{
		//탭 인덱스를 탭 타입에 따라 필터타입으로 컨버팅해서 필터를 처리함		
	}
}
