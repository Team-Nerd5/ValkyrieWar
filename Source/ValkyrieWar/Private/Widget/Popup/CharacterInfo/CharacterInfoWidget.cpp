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

		EventSystem->Widget.OnUpdateEquipment.AddDynamic(this, &UCharacterInfoWidget::UpdateEquipmentInventory);
		EventSystem->Widget.OnChangeEquipCharacter.AddDynamic(this, &UCharacterInfoWidget::UpdateEquipmentUi);
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


	EquipmentTileView->OnItemClicked().AddUObject(this, &UCharacterInfoWidget::ItemClicked);
}

void UCharacterInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();

	EventSystem->Widget.OnUpdateEquipment.RemoveDynamic(this, &UCharacterInfoWidget::UpdateEquipmentInventory);
	EventSystem->Widget.OnChangeEquipCharacter.RemoveDynamic(this, &UCharacterInfoWidget::UpdateEquipmentUi);
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
	CurrentEquipGroup = EEquipGroup::None;

	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip, CurrentEquipGroup);
	EquipmentTileView->SetListItems(CachedItemList);
	EquipmentTileView->RegenerateAllEntries();
}

void UCharacterInfoWidget::FilterWeapon()
{
	CurrentEquipGroup = EEquipGroup::Weapon;

	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip, CurrentEquipGroup);
	EquipmentTileView->SetListItems(CachedItemList);
	EquipmentTileView->RegenerateAllEntries();
}

void UCharacterInfoWidget::FilterArmor()
{
	CurrentEquipGroup = EEquipGroup::Armor;

	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip, CurrentEquipGroup);
	EquipmentTileView->SetListItems(CachedItemList);
	EquipmentTileView->RegenerateAllEntries();
}

void UCharacterInfoWidget::FilterHelmet()
{
	CurrentEquipGroup = EEquipGroup::Helmet;

	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip, CurrentEquipGroup);
	EquipmentTileView->SetListItems(CachedItemList);
	EquipmentTileView->RegenerateAllEntries();
}

void UCharacterInfoWidget::UpdateEquipmentInventory()
{
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
}

void UCharacterInfoWidget::UpdateEquipmentUi(uint64 InCharacterUID, EEquipGroup InEquipGroup)
{
	
}

void UCharacterInfoWidget::ItemClicked(UObject* InItemData)
{
	UItemData* ItemData = Cast<UItemData>(InItemData);

#pragma region 유효성 검사
	if (!ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[CharacterInfoWidget(ItemClicked)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	EquipButtonWidget->SetupEquipItem(ItemData);
}

void UCharacterInfoWidget::ItemClicked(UObject* InItemData)
{

}

void UCharacterInfoWidget::UpdateEquipmentUi(uint64 InCharacterUID, EEquipGroup InEquipGroup)
{

}
