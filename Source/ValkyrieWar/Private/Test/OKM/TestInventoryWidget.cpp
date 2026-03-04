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

	if(Btn_Close)
		Btn_Close->OnClicked.AddDynamic(this, &UTestInventoryWidget::CloseUI);

	if(Btn_Inventory)
		Btn_Inventory->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterInventory);
	if(Btn_Equipment)
		Btn_Equipment->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterEquipment);

	if(Btn_FilterReset)
		Btn_FilterReset->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterReset);
	if(Btn_FilterWeapon)
		Btn_FilterWeapon->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterWeapon);
	if(Btn_FilterArmor)
		Btn_FilterArmor->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterArmor);
	if(Btn_FilterHelmet)
		Btn_FilterHelmet->OnClicked.AddDynamic(this, &UTestInventoryWidget::FilterHelmet);
	if(Btn_FilterGrowth)
		Btn_FilterGrowth->OnClicked.AddDynamic(this, &UTestInventoryWidget::FIlterGrowth);
	if (Btn_FilterGoods)
		Btn_FilterGoods->OnClicked.AddDynamic(this, &UTestInventoryWidget::FIlterGoods);

	EventSystem->Widget.OnUpdateInventory.AddDynamic(this, &UTestInventoryWidget::FilterReset);
	EventSystem->Widget.OnUpdateCharacterEquipment.AddDynamic(this, &UTestInventoryWidget::UpdateEquipmentUi);

	TileView->OnItemClicked().AddUObject(this, &UTestInventoryWidget::ItemClicked);

	PopupWidget->SetVisibility(ESlateVisibility::Hidden);
	WeaponWidget->SetVisibility(ESlateVisibility::Hidden);
	HelmetWidget->SetVisibility(ESlateVisibility::Hidden);
	ArmorWidget->SetVisibility(ESlateVisibility::Hidden);

}

void UTestInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();

	EventSystem->Widget.OnUpdateInventory.RemoveDynamic(this, &UTestInventoryWidget::FilterReset);
	EventSystem->Widget.OnUpdateCharacterEquipment.RemoveDynamic(this, &UTestInventoryWidget::UpdateEquipmentUi);
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
	if (UIType == EUIType::PopupInventory)
		FilterInventory();
	else if (UIType == EUIType::PopupCharacterInfo)
		FilterEquipment();
	else
		return;
}

void UTestInventoryWidget::FilterInventory()
{
	UIType = EUIType::PopupInventory;

	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);
	SortInventory();

	PopupWidget->PopupSetHidden();
	WeaponWidget->SetVisibility(ESlateVisibility::Hidden);
	HelmetWidget->SetVisibility(ESlateVisibility::Hidden);
	ArmorWidget->SetVisibility(ESlateVisibility::Hidden);
	
}

void UTestInventoryWidget::FilterEquipment()	
{
	UIType = EUIType::PopupCharacterInfo;

	CachedItemList.Empty();
	//나눠서 넣는 이유..?
	CachedItemList.Append(InventorySystem->GetFilteredInventoryList(EItemGroup::Equip));
	SortInventory();

	PopupWidget->PopupSetHidden();
	WeaponWidget->SetVisibility(ESlateVisibility::Visible);
	HelmetWidget->SetVisibility(ESlateVisibility::Visible);
	ArmorWidget->SetVisibility(ESlateVisibility::Visible);
}


void UTestInventoryWidget::FilterWeapon()
{
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip, EEquipGroup::Weapon);
	SortInventory();
}

void UTestInventoryWidget::FilterArmor()
{
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip, EEquipGroup::Armor);
	SortInventory();
}

void UTestInventoryWidget::FilterHelmet()
{
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip, EEquipGroup::Helmet);
	SortInventory();
}

void UTestInventoryWidget::FIlterGrowth()
{	
	if (UIType == EUIType::PopupInventory)
	{
		CachedItemList.Empty();
		CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::GrowthItem);
		SortInventory();
	}
	else
	{
		CachedItemList.Empty();
		TileView->SetListItems(CachedItemList);
		TileView->RegenerateAllEntries();
	}	
}

void UTestInventoryWidget::FIlterGoods()
{
	if (UIType == EUIType::PopupInventory)
	{
		CachedItemList.Empty();
		CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Goods);
		SortInventory();
	}
	else
	{
		CachedItemList.Empty();
		TileView->SetListItems(CachedItemList);
		TileView->RegenerateAllEntries();
	}
}

void UTestInventoryWidget::ItemClicked(UObject* InItemData)
{
#pragma region 유효성 검사
	UItemData* ItemData = Cast<UItemData>(InItemData);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventoryWidget(EquipmentTileItemClicked)] ItemData가 없습니다"));
		return;
	}
#pragma endregion
	if (PopupWidget)
	{
		PopupWidget->InitMenu(ItemData, UIType);
		PopupWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTestInventoryWidget::UpdateEquipmentUi(uint64 InCharacterUID)
{
	UItemData* WeaponData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EEquipGroup::Weapon);
	if (WeaponWidget && WeaponData)
		WeaponWidget->EquipInfo(WeaponData);

	UItemData* HelmetData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EEquipGroup::Helmet);
	if (HelmetWidget && HelmetData)
		HelmetWidget->EquipInfo(HelmetData);

	UItemData* ArmorData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EEquipGroup::Armor);
	if (ArmorWidget && ArmorData)
		ArmorWidget->EquipInfo(ArmorData);
}

void UTestInventoryWidget::SortInventory()
{
	CachedItemList.Sort([](UItemData& A, UItemData& B)
		{
			const auto TableA = A.GetTableData();
			const auto TableB = B.GetTableData();

			if (A.GetItemGroup() == B.GetItemGroup())
			{
				return A.GetTableData().DataId < B.GetTableData().DataId;
			}

			if (A.GetItemGroup() == EItemGroup::None) return false;
			if (B.GetItemGroup() == EItemGroup::None) return true;

			return A.GetItemGroup() < B.GetItemGroup();
		});
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}
