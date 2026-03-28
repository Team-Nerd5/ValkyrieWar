// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/ValkyrieListWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "Components/ListView.h"

#include "Data/Game/ValkyrieData.h"

void UValkyrieListWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UValkyrieListWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UValkyrieListWidget::SetData(TArray<UValkyrieData*> InValkyries)
{
	CachedValkyries = InValkyries;

	if (ValkyrieListView)
	{
		ValkyrieListView->ClearSelection();
		ValkyrieListView->SetListItems(CachedValkyries);
	}
}

void UValkyrieListWidget::SelectValkyrie(UValkyrieData* InData)
{
	for (UValkyrieData* Data : CachedValkyries)
	{
		if (Data->GetUID() == InData->GetUID())
		{
			ValkyrieListView->SetItemSelection(Data, true);
			ValkyrieListView->RequestNavigateToItem(Data);
			break;
		}
	}
}
