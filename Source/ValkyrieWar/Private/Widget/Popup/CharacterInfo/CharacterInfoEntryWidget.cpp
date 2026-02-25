// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/CharacterInfoEntryWidget.h"
#include "Data/Game/ItemData.h"

void UCharacterInfoEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	Init(ListItemObject);
}

void UCharacterInfoEntryWidget::Init(UObject* InData)
{
	UItemData* ItemData = Cast<UItemData>(InData);
	if (!ItemData)
		return;

	UTexture2D* IconTexture = ItemData->GetTableData()->Icon.LoadSynchronous();
	if (IconTexture)
		Icon->SetBrushFromTexture(IconTexture);
}
