// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Popup/Inventory/InventoryWidget.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
	}

	if (Btn_FilterReset)
		Btn_FilterReset->OnClicked.AddDynamic(this, &UInventoryWidget::FilterReset);
	if (Btn_FilterWeapon)
		Btn_FilterWeapon->OnClicked.AddDynamic(this, &UInventoryWidget::FilterWeapon);
	if (Btn_FilterArmor)
		Btn_FilterArmor->OnClicked.AddDynamic(this, &UInventoryWidget::FilterArmor);
	if (Btn_FilterHelmet)
		Btn_FilterHelmet->OnClicked.AddDynamic(this, &UInventoryWidget::FilterHelmet);
	if (Btn_FilterGrowth)
		Btn_FilterGrowth->OnClicked.AddDynamic(this, &UInventoryWidget::FilterGrowth);
	if (Btn_FilterGoods)
		Btn_FilterGoods->OnClicked.AddDynamic(this, &UInventoryWidget::FilterGoods);

	InventoryTileView->OnItemClicked().AddUObject(this, &UInventoryWidget::OnItemClicked);

	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.AddDynamic(this, &UInventoryWidget::OnUpdateInventory);
	}
}

void UInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.RemoveDynamic(this, &UInventoryWidget::OnUpdateInventory);
	}
}

void UInventoryWidget::OpenUI()
{
	Super::OpenUI();
}

void UInventoryWidget::CloseUI()
{
	Super::CloseUI();
}

void UInventoryWidget::UpdateInventoryType(EUIType InUIType)
{
}

void UInventoryWidget::FilterReset()
{
	CurrentItemGroup = EItemGroup::None;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup);
	InventoryTileView->SetListItems(CachedItemList);
	InventoryTileView->RegenerateAllEntries();
}

void UInventoryWidget::FilterWeapon()
{
	CurrentItemGroup = EItemGroup::Equip;
	CurrentEquipGroup = EEquipGroup::Weapon;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup, CurrentEquipGroup);
	InventoryTileView->SetListItems(CachedItemList);
	InventoryTileView->RegenerateAllEntries();
}

void UInventoryWidget::FilterArmor()
{
	CurrentItemGroup = EItemGroup::Equip;
	CurrentEquipGroup = EEquipGroup::Armor;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup, CurrentEquipGroup);
	InventoryTileView->SetListItems(CachedItemList);
	InventoryTileView->RegenerateAllEntries();
}

void UInventoryWidget::FilterHelmet()
{
	CurrentItemGroup = EItemGroup::Equip;
	CurrentEquipGroup = EEquipGroup::Helmet;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup, CurrentEquipGroup);
	InventoryTileView->SetListItems(CachedItemList);
	InventoryTileView->RegenerateAllEntries();
}

void UInventoryWidget::FilterGrowth()
{
	CurrentItemGroup = EItemGroup::GrowthItem;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup);
	InventoryTileView->SetListItems(CachedItemList);
	InventoryTileView->RegenerateAllEntries();
}

void UInventoryWidget::FilterGoods()
{
	CurrentItemGroup = EItemGroup::Goods;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup);
	InventoryTileView->SetListItems(CachedItemList);
	InventoryTileView->RegenerateAllEntries();
}

void UInventoryWidget::OnUpdateInventory()
{
	switch (CurrentItemGroup)
	{
	case EItemGroup::Goods:
		FilterGoods();
		break;
	case EItemGroup::GrowthItem:
		FilterGrowth();
		break;
	case EItemGroup::Equip:
		switch (CurrentEquipGroup)
		{
		case EEquipGroup::Weapon:
			FilterWeapon();
			break;
		case EEquipGroup::Helmet:
			FilterHelmet();
			break;
		case EEquipGroup::Armor:
			FilterArmor();
			break;
		default:
			FilterReset();
			break;
		}
		break;
	default:
		FilterReset();
		break;
	}
}

void UInventoryWidget::OnUpdateEquipmentForUID(uint64 InCharacterUID)
{
}

void UInventoryWidget::OnItemClicked(UObject* InItemData)
{
	UItemData* ItemData = Cast<UItemData>(InItemData);

#pragma region 유효성 검사
	if (!ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventoryWidget(ItemClicked)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	//SellButtonWidget->SetupSellItem(ItemData);
}

void UInventoryWidget::RefreshUIByMode()
{
}
