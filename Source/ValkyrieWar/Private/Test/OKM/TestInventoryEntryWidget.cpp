// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestInventoryEntryWidget.h"

#include "Test/OKM/TestItemObject.h"

#include "Data/Game/ItemData.h"

void UTestInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	//TestInit(ListItemObject);

	Init(ListItemObject);
}

void UTestInventoryEntryWidget::Init(UObject* InData)
{
	UItemData* ItemData = Cast<UItemData>(InData);
	if (!ItemData)
		return;

	Amount->SetText(FText::AsNumber(ItemData->GetAmount()));
	UID->SetText(FText::AsNumber(ItemData->GetUID()));
	DataID->SetText(FText::AsNumber(ItemData->GetTableData()->DataId));
	FText ItemGroupText = StaticEnum<EItemGroup>()->GetDisplayNameTextByValue(static_cast<int64>(ItemData->GetTableData()->ItemGroup));
	ItemGroup->SetText(ItemGroupText);
}

void UTestInventoryEntryWidget::RefreshText()
{
	
}
