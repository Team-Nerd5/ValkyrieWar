// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestEquipmentWidget.h"

void UTestEquipmentWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTestEquipmentWidget::EquipInfo(UItemData* InItemData)
{
	if (!InItemData)
	{
		EquipCharacterUID->SetText(FText::AsNumber(0));
	}

	if (InItemData)
	{
		UID->SetText(FText::AsNumber(InItemData->GetUID()));

		DataID->SetText(FText::AsNumber(InItemData->GetTableData()->DataId));

		FText ItemGroupText = StaticEnum<EItemGroup>()->GetDisplayNameTextByValue(static_cast<int64>(InItemData->GetTableData()->ItemGroup));
		ItemGroup->SetText(ItemGroupText);

		EquipCharacterUID->SetText(FText::AsNumber(InItemData->GetEquipCharacter()));
	}
}
