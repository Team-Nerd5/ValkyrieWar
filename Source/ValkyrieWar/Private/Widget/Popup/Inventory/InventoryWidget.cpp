// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
		EventSystem = World->GetSubsystem<UWorldEventSystem>();
	}

	if (Btn_Close)
		Btn_Close->OnClicked.AddDynamic(this, &UInventoryWidget::CloseUI);

	if (Btn_FilterReset)
		Btn_FilterReset->OnClicked.AddDynamic(this, &UInventoryWidget::FilterReset);
	if (Btn_FilterWeapon)
		Btn_FilterWeapon->OnClicked.AddDynamic(this, &UInventoryWidget::FilterWeapon);
	if (Btn_FilterArmor)
		Btn_FilterArmor->OnClicked.AddDynamic(this, &UInventoryWidget::FilterArmor);
	if (Btn_FilterHelmet)
		Btn_FilterHelmet->OnClicked.AddDynamic(this, &UInventoryWidget::FilterHelmet);
	if (Btn_FilterGrowth)
		Btn_FilterGrowth->OnClicked.AddDynamic(this, &UInventoryWidget::FIlterGrowth);
	if (Btn_FilterGoods)
		Btn_FilterGoods->OnClicked.AddDynamic(this, &UInventoryWidget::FIlterGoods);

	EventSystem->Widget.OnUpdateInventory.AddDynamic(this, &UInventoryWidget::FilterReset);
	EventSystem->Widget.OnChangeEquipCharacter.AddDynamic(this, &UInventoryWidget::UpdateEquipmentUi);

	TileView->OnItemClicked().AddUObject(this, &UInventoryWidget::ItemClicked);
}

void UInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UInventoryWidget::OpenUI()
{
	Super::OpenUI();
}

void UInventoryWidget::CloseUI()
{
	Super::CloseUI();
}

void UInventoryWidget::FilterReset()
{

}

void UInventoryWidget::FilterWeapon()
{

}

void UInventoryWidget::FilterArmor()
{

}

void UInventoryWidget::FilterHelmet()
{

}

void UInventoryWidget::FIlterGrowth()
{

}

void UInventoryWidget::FIlterGoods()
{

}

void UInventoryWidget::ItemClicked(UObject* InItemData)
{

}

void UInventoryWidget::UpdateEquipmentUi(uint64 InCharacterUID, EEquipGroup InEquipGroup)
{

}
