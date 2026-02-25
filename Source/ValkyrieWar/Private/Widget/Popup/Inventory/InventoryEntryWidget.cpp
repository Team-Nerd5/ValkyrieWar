// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryEntryWidget.h"
#include "Data/Game/ItemData.h"

void UInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	Init(ListItemObject);
}

void UInventoryEntryWidget::Init(UObject* InData)
{
	UItemData* ItemData = Cast<UItemData>(InData);
	if (!ItemData)
		return;

	if (ItemData->GetItemGroup() == EItemGroup::Goods || ItemData->GetItemGroup() == EItemGroup::GrowthItem)
		Amount->SetText(FText::AsNumber(ItemData->GetAmount()));
	else
		Amount->SetText(FText::AsNumber(0));

	UTexture2D* IconTexture = ItemData->GetTableData()->Icon.LoadSynchronous();
	if(IconTexture)
		Icon->SetBrushFromTexture(IconTexture);

}
