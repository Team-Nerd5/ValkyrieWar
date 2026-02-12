// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestInventoryWidget.h"
#include "Data/Enum/DataEnums.h"

void UTestInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetGameInstance()->GetSubsystem<UInventorySystem>();
	}
	InventorySystem = GetWorld()->GetSubsystem<UInventorySystem>();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);

	CloseButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::CloseUI);

	FilterResetButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterReset);
	FilterWeaponButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterWeapon);
	FilterArmorButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterArmor);
	FilterHelmetButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterHelmet);
	FilterGrowthButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FIlterGrowth);
}

void UTestInventoryWidget::OpenUI()
{
	Super::OpenUI();
}

void UTestInventoryWidget::CloseUI()
{
	Super::CloseUI();
}

void UTestInventoryWidget::FilterReset()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FilterWeapon()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Weapon);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FilterArmor()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Armor);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FilterHelmet()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Helmet);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FIlterGrowth()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Growth);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}


