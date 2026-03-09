// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryWidget.h"

#include "GameSystem/Library/GameDataHelper.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "Widget/HUD/TopMenuWidget.h"
#include "Widget/Popup/Inventory/ItemListWidget.h"
#include "Algo/Sort.h"


void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EventSystem)
	{
		//TODO : 아이템이 새로 추가되거나 제거된 경우는 이벤트를 받아서 리스트 자체를 갱신
		//열려있을 때만 갱신해도 될 것 같음
	}


}

void UInventoryWidget::NativeDestruct()
{
	if (EventSystem)
	{
		/*EventSystem->Widget.OnUpdateInventory.RemoveDynamic(this, &UInventoryWidget::OnUpdateInventory);
		EventSystem->Widget.OnUpdateCharacterEquipment.RemoveDynamic(this, &UInventoryWidget::OnUpdateEquipmentForUID);
		EventSystem->Widget.OnUpdateInventorySelectedCancel.RemoveDynamic(this, &UInventoryWidget::OnUpdateSelectedCancel);*/
	}

	Super::NativeDestruct();
}

void UInventoryWidget::OpenUI()
{
	UIType = EUIType::PopupInventory;

	Super::OpenUI();

	//인벤토리 시스템에서 아이템 가져와서 기본 데이터 세팅
	if (UInventorySystem* Inventory = GetGameInstance()->GetSubsystem<UInventorySystem>())
	{
		OriginItems = Inventory->GetAllItems();
	}

	InitItemList();
}

void UInventoryWidget::CloseUI()
{
	Super::CloseUI();
}


void UInventoryWidget::InitItemList()
{
	if (ItemListWidget)
	{
		ItemListWidget->SetMenu(InventoryTabNameData);
		ItemListWidget->SetData(OriginItems);
	}
}

void UInventoryWidget::OnItemClicked(UObject* InItemData)
{
	UItemData* ItemData = Cast<UItemData>(InItemData);

}

//void UInventoryWidget::OnCharacterClicked(UObject* InCharacterData)
//{
//	// 캐릭터 데이터 캐스트
//
//	// 캐릭터 선택 시
//	// 메인 캐릭터 선택 버튼 표시 -> 버튼 눌렀을 때 매인 케릭터 저장
//	// 캐릭터가 장착중인 장비 -> 장비 슬롯에 표시
//
//	// 캐릭터 선택 시 장비 버튼 표시 이후 버튼 눌렀을 때 장비 인벤토리 표시하도록 해야하나?
//	
//}

void UInventoryWidget::OnClickClose()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->CloseTopPopupUI();
	}
}

void UInventoryWidget::SortInventory()
{
	//Algo::Sort(CachedItemList, [](UItemData* ItemA, UItemData* ItemB) {
	//		if (!ItemA && !ItemB) return false;
	//		if (!ItemA) return false;
	//		if (!ItemB) return true;

	//		if (ItemA->GetItemGroup() != ItemB->GetItemGroup())
	//		{
	//			if (ItemA->GetItemGroup() == EItemGroup::None) return false;		// A의 ItemGroup이 None이면 뒤로
	//			if (ItemB->GetItemGroup() == EItemGroup::None) return true;			// B의 ItemGroup이 None이면 A를 앞으로

	//			return ItemA->GetItemGroup() < ItemB->GetItemGroup();				// ItemGroup Enum 순서대로 정렬
	//		}

	//		if (ItemA->GetEquipGroup() != ItemB->GetEquipGroup())
	//		{
	//			if (ItemA->GetEquipGroup() == EEquipGroup::None) return false;		// A의 EquipGroup이 None이면 뒤로
	//			if (ItemB->GetEquipGroup() == EEquipGroup::None) return true;		// B의 EquipGroup이 None이면 A를 앞으로

	//			return ItemA->GetEquipGroup() < ItemB->GetEquipGroup();				// EquipGroup Enum 순서대로 정렬
	//		}

	//		const auto TableDataA = ItemA->GetTableData();
	//		const auto TableDataB = ItemB->GetTableData();

	//		return TableDataA.DataId < TableDataB.DataId;							// A와 B의 ItemGroup과 EquipGroup이 같을 때 DataId 오름차순으로 정렬
	//	});

}
