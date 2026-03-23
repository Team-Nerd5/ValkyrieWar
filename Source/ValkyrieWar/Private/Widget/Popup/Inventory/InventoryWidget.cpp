// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryWidget.h"

#include "GameSystem/Library/GameDataHelper.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "Widget/HUD/TopMenuWidget.h"
#include "Widget/Popup/Inventory/ItemListWidget.h"



void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.AddDynamic(this, &UInventoryWidget::OnInventoryUpdate);
		EventSystem->Widget.OnTabMenuSelected.AddDynamic(this, &UInventoryWidget::OnTabMenuChanged);
	}
}

void UInventoryWidget::NativeDestruct()
{
	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.RemoveDynamic(this, &UInventoryWidget::OnInventoryUpdate);
		EventSystem->Widget.OnTabMenuSelected.RemoveDynamic(this, &UInventoryWidget::OnTabMenuChanged);
	}

	Super::NativeDestruct();
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CurrentFilterType = EInventoryFilterType::All;

	if (ItemListWidget)
	{
		ItemListWidget->SetMenu(InventoryTabNameData);
	}
	InitItemList();
}

void UInventoryWidget::OpenUI()
{
	UIType = EUIType::PopupInventory;

	Super::OpenUI();

	CurrentFilterType = EInventoryFilterType::All;
	if (ItemListWidget)
	{
		ItemListWidget->InitFilterIndex(static_cast<int32>(CurrentFilterType));
	}
}

void UInventoryWidget::CloseUI()
{
	Super::CloseUI();
}

void UInventoryWidget::InitItemList()
{
	//인벤토리 시스템에서 아이템 가져와서 기본 데이터 세팅
	if (UInventorySystem* Inventory = GetGameInstance()->GetSubsystem<UInventorySystem>())
	{
		OriginItems = Inventory->GetAllItems();
	}

	if (ItemListWidget)
	{
		ItemListWidget->SetData(OriginItems);
	}
}

void UInventoryWidget::OnItemClicked(UObject* InItemData)
{
	UItemData* ItemData = Cast<UItemData>(InItemData);

}

void UInventoryWidget::OnClickClose()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->CloseTopPopupUI();
	}
}

void UInventoryWidget::OnInventoryUpdate()
{
	if (bIsOpen)
	{
		InitItemList();
	}
}

void UInventoryWidget::OnTabMenuChanged(int32 InSelectedTab)
{
	if (ItemListWidget)
	{
		ItemListWidget->InitFilterIndex(InSelectedTab);
		//탭 바뀌면 선택 아이템을 비워줌
		ItemListWidget->OnItemSelected(nullptr);
	}
}
