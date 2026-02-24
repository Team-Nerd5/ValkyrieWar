// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/CharacterInfoEntryWidget.h"

void UCharacterInfoEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	Init(ListItemObject);
}

void UCharacterInfoEntryWidget::Init(UObject* InData)
{

}
