// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/CharacterInfoWidget.h"

void UCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
		EventSystem = World->GetSubsystem<UWorldEventSystem>();
	}

	if (Btn_Close)
		Btn_Close->OnClicked.AddDynamic(this, &UCharacterInfoWidget::CloseUI);

	if (Btn_FilterReset)
		Btn_FilterReset->OnClicked.AddDynamic(this, &UCharacterInfoWidget::FilterReset);
	if (Btn_FilterWeapon)
		Btn_FilterWeapon->OnClicked.AddDynamic(this, &UCharacterInfoWidget::FilterWeapon);
	if (Btn_FilterArmor)
		Btn_FilterArmor->OnClicked.AddDynamic(this, &UCharacterInfoWidget::FilterArmor);
	if (Btn_FilterHelmet)
		Btn_FilterHelmet->OnClicked.AddDynamic(this, &UCharacterInfoWidget::FilterHelmet);

	EventSystem->Widget.OnUpdateInventory.AddDynamic(this, &UCharacterInfoWidget::FilterReset);
	EventSystem->Widget.OnChangeEquipCharacter.AddDynamic(this, &UCharacterInfoWidget::UpdateEquipmentUi);

	TileView->OnItemClicked().AddUObject(this, &UCharacterInfoWidget::ItemClicked);
}

void UCharacterInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UCharacterInfoWidget::OpenUI()
{
	Super::OpenUI();
}

void UCharacterInfoWidget::CloseUI()
{
	Super::CloseUI();
}

void UCharacterInfoWidget::FilterReset()
{

}

void UCharacterInfoWidget::FilterWeapon()
{

}

void UCharacterInfoWidget::FilterArmor()
{

}

void UCharacterInfoWidget::FilterHelmet()
{

}
