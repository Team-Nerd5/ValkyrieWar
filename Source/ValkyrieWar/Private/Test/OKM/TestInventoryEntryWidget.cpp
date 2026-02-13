// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestInventoryEntryWidget.h"

#include "Test/OKM/TestItemObject.h"

#include "Data/Game/ItemData.h"

void UTestInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	TestInit(ListItemObject);

	//Init(ListItemObject);
}

void UTestInventoryEntryWidget::TestInit(UObject* InData)
{
	UTestItemObject* ItemData = Cast<UTestItemObject>(InData);
	if (!ItemData)
		return;

	Amount->SetText(FText::AsNumber(ItemData->GetTestObjectAmount()));
	ID->SetText(FText::AsNumber(ItemData->GetTestObjectID()));
}

void UTestInventoryEntryWidget::Init(UObject* InData)
{
	UItemData* ItemData = Cast<UItemData>(InData);
	if (!ItemData)
		return;

	Amount->SetText(FText::AsNumber(ItemData->GetAmount()));
}
