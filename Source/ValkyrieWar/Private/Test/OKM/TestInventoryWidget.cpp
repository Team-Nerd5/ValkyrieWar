// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestInventoryWidget.h"
#include "Data/Enum/DataEnums.h"

void UTestInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
		EventSystem = World->GetSubsystem<UWorldEventSystem>();
	}

	if(CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::CloseUI);

	if(FilterResetButton)
		FilterResetButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterReset);
	if(FilterWeaponButton)
		FilterWeaponButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterWeapon);
	if(FilterArmorButton)
		FilterArmorButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterArmor);
	if(FilterHelmetButton)
		FilterHelmetButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterHelmet);
	if(FilterGrowthButton)
		FilterGrowthButton->OnClicked.AddDynamic(this, &UTestInventoryWidget::FIlterGrowth);

	EventSystem->Widget.OnChangeItemAmount.AddDynamic(this, &UTestInventoryWidget::FilterReset);

	FilterReset();
	
}

void UTestInventoryWidget::OpenUI()
{
	Super::OpenUI();

	FilterReset();
}

void UTestInventoryWidget::CloseUI()
{
	Super::CloseUI();
}

void UTestInventoryWidget::FilterReset()
{
	TileView->ClearListItems();
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);
	TileView->SetListItems(CachedItemList);
}

void UTestInventoryWidget::FilterWeapon()
{
	TileView->ClearListItems();
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Weapon);
	TileView->SetListItems(CachedItemList);
}

void UTestInventoryWidget::FilterArmor()
{
	TileView->ClearListItems();
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Armor);
	TileView->SetListItems(CachedItemList);
}

void UTestInventoryWidget::FilterHelmet()
{
	TileView->ClearListItems();
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Helmet);
	TileView->SetListItems(CachedItemList);
}

void UTestInventoryWidget::FIlterGrowth()
{
	TileView->ClearListItems();
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Growth);
	TileView->SetListItems(CachedItemList);
}


