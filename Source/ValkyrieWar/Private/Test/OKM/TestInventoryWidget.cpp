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

	EventSystem->Widget.OnChangeItemAmount.AddDynamic(this, &UTestInventoryWidget::FilterReset);
	EventSystem->Widget.OnChangeEquipCharacter.AddDynamic(this, &UTestInventoryWidget::FilterReset);

	TileView->OnItemClicked().AddUObject(this, &UTestInventoryWidget::ItemClicked);

	PopupWidget->SetVisibility(ESlateVisibility::Collapsed);

	if (Money)
		Money->SetText(FText::AsNumber(InventorySystem->GetMoney()));

	FilterReset();
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
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}

void UTestInventoryWidget::FilterWeapon()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Weapon);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}

void UTestInventoryWidget::FilterArmor()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Armor);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}

void UTestInventoryWidget::FilterHelmet()
{
	CachedItemList.Empty();
	CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Helmet);
	TileView->SetListItems(CachedItemList);
	TileView->RegenerateAllEntries();
}

void UTestInventoryWidget::FIlterGrowth()
{
	CachedItemList.Empty();
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
		PopupWidget->InitMenu(ItemData);
		PopupWidget->SetVisibility(ESlateVisibility::Visible);
	}

}


