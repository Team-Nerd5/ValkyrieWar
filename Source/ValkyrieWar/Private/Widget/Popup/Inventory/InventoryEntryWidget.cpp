// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryEntryWidget.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "Data/Game/ItemData.h"

void UInventoryEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnInventoryItemAmountChanged.AddDynamic(this, &UInventoryEntryWidget::OnAmountChanged);
	}
}

void UInventoryEntryWidget::NativeDestruct()
{
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnInventoryItemAmountChanged.RemoveDynamic(this, &UInventoryEntryWidget::OnAmountChanged);
	}

	Super::NativeDestruct();
}

void UInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	//??
	//IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	if (SelectImage)
		SelectImage->SetVisibility(ESlateVisibility::Hidden);

	UItemData* ItemData = Cast<UItemData>(ListItemObject);

	if (ItemData)
	{
		CachedItemData = ItemData;
		Init(ItemData);
	}
}

void UInventoryEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (!CachedItemData)
		return;

	if (SelectImage)
	{
		SelectImage->SetVisibility(bIsSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}

	if (IsSelected && !bIsSelected)
	{
		//선택되어 있다가 꺼짐
		if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			WorldEventSystem->Widget.OnInventoryItemSelected.Broadcast(nullptr);
		}

		IsSelected = bIsSelected;

		return;
	}

	if (bIsSelected)
	{
		//그냥 선택됨
		if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			WorldEventSystem->Widget.OnInventoryItemSelected.Broadcast(CachedItemData);
		}

		IsSelected = bIsSelected;
	}
}

void UInventoryEntryWidget::Init(UItemData* InData)
{
	if (!InData)
		return;
	
	if (Amount)
	{
		if (InData->GetItemGroup() == EItemGroup::GrowthItem)
		{
			Amount->SetText(FText::AsNumber(InData->GetAmount()));
			Amount->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Amount->SetText(FText::AsNumber(0));
			Amount->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (InData->GetTableData().DataId > 0)
	{
		UTexture2D* IconTexture = InData->GetTableData().Icon.LoadSynchronous();
		if(IconTexture)
			Icon->SetBrushFromTexture(IconTexture);
	}
}

void UInventoryEntryWidget::OnAmountChanged(uint64 InUID)
{
	if (CachedItemData && CachedItemData->GetUID() == InUID)
	{
		if (Amount)
		{
			Amount->SetText(FText::AsNumber(CachedItemData->GetAmount()));
		}
	}
}
