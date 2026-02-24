// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryEntryWidget.h"

void UInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	Init(ListItemObject);
}

void UInventoryEntryWidget::Init(UObject* InData)
{

}
