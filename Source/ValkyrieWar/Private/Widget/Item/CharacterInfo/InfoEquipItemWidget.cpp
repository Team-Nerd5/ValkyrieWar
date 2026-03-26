// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/CharacterInfo/InfoEquipItemWidget.h"
#include "Components/Image.h"

void UInfoEquipItemWidget::InitEquip(TSoftObjectPtr<UTexture2D> InIcon)
{
	if (ItemIcon)
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);

	if (TypeIcon)
	{
		TypeIcon->SetBrushFromSoftTexture(InIcon);
	}
}

void UInfoEquipItemWidget::SetEquip(TSoftObjectPtr<UTexture2D> InIcon)
{
	if (InIcon.IsValid())
	{
		ItemIcon->SetBrushFromSoftTexture(InIcon);
	}

	if (TypeIcon)
		TypeIcon->SetVisibility(InIcon.IsValid() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

	if(ItemIcon)
		ItemIcon->SetVisibility(InIcon.IsValid() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
