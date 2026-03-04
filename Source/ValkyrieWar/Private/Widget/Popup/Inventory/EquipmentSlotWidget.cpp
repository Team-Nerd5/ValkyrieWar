// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/EquipmentSlotWidget.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WeaponIcon->SetVisibility(ESlateVisibility::Hidden);
	HelmetIcon->SetVisibility(ESlateVisibility::Hidden);
	ArmorIcon->SetVisibility(ESlateVisibility::Hidden);
}

void UEquipmentSlotWidget::RefreshEquipment(uint64 InCharacterUID)
{
	if (UInventorySystem* InventorySystem = GetGameInstance()->GetSubsystem<UInventorySystem>())
	{
		if (WeaponIcon)
		{
			UItemData* WeaponData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EEquipGroup::Weapon);
			if (WeaponData)
			{
				WeaponIcon->SetVisibility(ESlateVisibility::Visible);
				UTexture2D* IconTexture = WeaponData->GetTableData().Icon.LoadSynchronous();
				if (IconTexture)
					WeaponIcon->SetBrushFromTexture(IconTexture);
			}
			else
			{
				WeaponIcon->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (HelmetIcon)
		{
			UItemData* HelmetData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EEquipGroup::Helmet);
			if (HelmetData)
			{
				HelmetIcon->SetVisibility(ESlateVisibility::Visible);
				UTexture2D* IconTexture = HelmetData->GetTableData().Icon.LoadSynchronous();
				if (IconTexture)
					HelmetIcon->SetBrushFromTexture(IconTexture);
			}
			else
			{
				HelmetIcon->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (ArmorIcon)
		{
			UItemData* ArmorData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EEquipGroup::Armor);
			if (ArmorData)
			{
				ArmorIcon->SetVisibility(ESlateVisibility::Visible);
				UTexture2D* IconTexture = ArmorData->GetTableData().Icon.LoadSynchronous();
				if (IconTexture)
					ArmorIcon->SetBrushFromTexture(IconTexture);
			}
			else
			{
				ArmorIcon->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}
