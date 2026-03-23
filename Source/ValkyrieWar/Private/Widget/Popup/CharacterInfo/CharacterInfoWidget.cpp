// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/CharacterInfoWidget.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "Widget/HUD/LobbyWidget.h"
#include "Widget/Popup/Inventory/ItemListWidget.h"

void UCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.AddUniqueDynamic(this, &UCharacterInfoWidget::OnInventoryUpdate);
		EventSystem->Widget.OnTabMenuSelected.AddUniqueDynamic(this, &UCharacterInfoWidget::OnTabMenuChanged);
	}
}

void UCharacterInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.RemoveDynamic(this, &UCharacterInfoWidget::OnInventoryUpdate);
		EventSystem->Widget.OnTabMenuSelected.RemoveDynamic(this, &UCharacterInfoWidget::OnTabMenuChanged);
	}
}

void UCharacterInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CurrentFilterType = ECharacterInfoFilterType::All;

	if (ItemListWidget)
	{
		ItemListWidget->SetMenu(InventoryTabNameData);
	}
	InitItemList();
}

void UCharacterInfoWidget::OpenUI()
{
	UIType = EUIType::PopupCharacterInfo;

	Super::OpenUI();

	CurrentFilterType = ECharacterInfoFilterType::All;
	if (ItemListWidget)
	{
		ItemListWidget->InitFilterIndex(static_cast<int32>(CurrentFilterType));
	}

}

void UCharacterInfoWidget::CloseUI()
{
	//로비 켜줘야함
	if (EventSystem)
	{
		EventSystem->Widget.OnCharacterInfoWidgetClosed.Broadcast();
	}
	Super::CloseUI();
}

void UCharacterInfoWidget::InitItemList()
{
	//인벤토리 시스템에서 아이템 가져와서 기본 데이터 세팅
	if (UInventorySystem* Inventory = GetGameInstance()->GetSubsystem<UInventorySystem>())
	{
		OriginItems = Inventory->GetEquipItems();
	}

	if (ItemListWidget)
	{
		ItemListWidget->SetData(OriginItems);
	}
}
void UCharacterInfoWidget::OnItemClicked(UObject* InItemData)
{
	UItemData* ItemData = Cast<UItemData>(InItemData);

}
void UCharacterInfoWidget::OnInventoryUpdate()
{
	if (bIsOpen)
	{
		InitItemList();
	}
}

void UCharacterInfoWidget::OnTabMenuChanged(int32 InSelectedTab)
{
	if (ItemListWidget)
	{
		ItemListWidget->InitFilterIndex(InSelectedTab);
		//탭 바뀌면 선택 아이템을 비워줌
		ItemListWidget->OnItemSelected(nullptr);
	}
}
