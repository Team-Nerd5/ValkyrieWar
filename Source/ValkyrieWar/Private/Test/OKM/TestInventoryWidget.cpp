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

	EventSystem->Widget.OnUpdateInventory.AddDynamic(this, &UTestInventoryWidget::FilterReset);
	EventSystem->Widget.OnChangeEquipCharacter.AddDynamic(this, &UTestInventoryWidget::UpdateEquipmentUi);

	TileView->OnItemClicked().AddUObject(this, &UTestInventoryWidget::ItemClicked);

	PopupWidget->SetVisibility(ESlateVisibility::Hidden);
	WeaponWidget->SetVisibility(ESlateVisibility::Hidden);
	HelmetWidget->SetVisibility(ESlateVisibility::Hidden);
	ArmorWidget->SetVisibility(ESlateVisibility::Hidden);

	CloseUI();
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
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();

	PopupWidget->PopupSetHidden();
	WeaponWidget->SetVisibility(ESlateVisibility::Hidden);
	HelmetWidget->SetVisibility(ESlateVisibility::Hidden);
	ArmorWidget->SetVisibility(ESlateVisibility::Hidden);
	
}

void UTestInventoryWidget::FilterEquipment()	
{
	UIType = EUIType::PopupCharacterInfo;

	CachedItemList.Empty();
	CachedItemList.Append(InventorySystem->GetFilteredInventoryList(EItemGroup::Weapon));
	CachedItemList.Append(InventorySystem->GetFilteredInventoryList(EItemGroup::Armor));
	CachedItemList.Append(InventorySystem->GetFilteredInventoryList(EItemGroup::Helmet));
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();

	PopupWidget->PopupSetHidden();
	WeaponWidget->SetVisibility(ESlateVisibility::Visible);
	HelmetWidget->SetVisibility(ESlateVisibility::Visible);
	ArmorWidget->SetVisibility(ESlateVisibility::Visible);
}

void UTestInventoryWidget::UpdateEquipmentUi(uint64 InCharacterUID)
{
	UItemData* WeaponData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EItemGroup::Weapon);
	UItemData* HelmetData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EItemGroup::Helmet);
	UItemData* ArmorData = InventorySystem->GetEquippedItemByGroup(InCharacterUID, EItemGroup::Armor);

	if (WeaponWidget)
		WeaponWidget->EquipInfo(WeaponData);

	if (HelmetWidget)
		HelmetWidget->EquipInfo(HelmetData);

	if (ArmorWidget)
		ArmorWidget->EquipInfo(ArmorData);

	return;
}

void UTestInventoryWidget::FilterWeapon()
{
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Weapon);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}

void UTestInventoryWidget::FilterArmor()
{
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Armor);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}

void UTestInventoryWidget::FilterHelmet()
{
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Helmet);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}

void UTestInventoryWidget::FIlterGrowth()
{	
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Growth);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
	
}

void UTestInventoryWidget::ItemClicked(UObject* InItemData)
{
#pragma region 유효성 검사
	UItemData* ItemData = Cast<UItemData>(InItemData);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventoryWidget(ItemClicked)] ItemData가 없습니다"));
		return;
	}
#pragma endregion
	if (PopupWidget)
	{
		PopupWidget->InitMenu(ItemData, UIType);
		PopupWidget->SetVisibility(ESlateVisibility::Visible);
	}
}




