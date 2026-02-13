// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestInventoryWidget.h"
#include "Data/Enum/DataEnums.h"

void UTestInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();

	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);

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
	TileView->ClearListItems();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FilterWeapon()
{
	TileView->ClearListItems();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Weapon);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FilterArmor()
{
	TileView->ClearListItems();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Armor);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FilterHelmet()
{
	TileView->ClearListItems();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Helmet);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::FIlterGrowth()
{
	TileView->ClearListItems();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Growth);
	for (UItemData* Item : CachedItemList)
	{
		TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::InitTestList()
{
	TileView->ClearListItems();
	for (UTestItemObject* Item : TestList)
	{
		if(Item)
			TileView->AddItem(Item);
	}
}

void UTestInventoryWidget::AddTestList(UObject* InItemObject)
{
	UTestItemObject* ItemObject = Cast<UTestItemObject>(InItemObject);
	if (!ItemObject)
	{
		UE_LOG(LogTemp, Log, TEXT("InItemObject가 UTestItemObject가 아닙니다"))
		return;
	}

	if (ItemObject)
	{
		TestList.Add(ItemObject);
	}
}


