// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryEntryWidget.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "Data/Game/ItemData.h"

void UInventoryEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnUpdateInventoryAmountChanged.AddDynamic(this, &UInventoryEntryWidget::HandleAmountChanged);
	}
}

void UInventoryEntryWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnUpdateInventoryAmountChanged.RemoveDynamic(this, &UInventoryEntryWidget::HandleAmountChanged);
	}

}

void UInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UItemData* ItemData = Cast<UItemData>(ListItemObject);

	if (ItemData)
	{
		CachedItemData = ItemData;
		Init(ItemData);
	}
}

void UInventoryEntryWidget::Init(UItemData* InData)
{
	if (InData->GetItemGroup() == EItemGroup::Goods || InData->GetItemGroup() == EItemGroup::GrowthItem)
	{
		Amount->SetText(FText::AsNumber(InData->GetAmount()));
		Amount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Amount->SetText(FText::AsNumber(0));
		Amount->SetVisibility(ESlateVisibility::Hidden);
	}

	if (InData->GetTableData().DataId > 0)
	{
		UTexture2D* IconTexture = InData->GetTableData().Icon.LoadSynchronous();
		if(IconTexture)
			Icon->SetBrushFromTexture(IconTexture);
	}
}

void UInventoryEntryWidget::HandleAmountChanged()
{
	if (CachedItemData)
	{
		Amount->SetText(FText::AsNumber(CachedItemData->GetAmount()));
	}
}
