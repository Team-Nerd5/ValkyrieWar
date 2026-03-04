// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/InventoryWidget.h"

#include "GameSystem/Library/GameDataHelper.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "Widget/HUD/TopMenuWidget.h"
#include "Algo/Sort.h"


void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
	}
	WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this);

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
	if (BackButton)
		BackButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnClickClose);

	if (WorldEventSystem)
	{
		WorldEventSystem->Widget.OnUpdateInventory.AddDynamic(this, &UInventoryWidget::OnUpdateInventory);
		WorldEventSystem->Widget.OnUpdateCharacterEquipment.AddDynamic(this, &UInventoryWidget::OnUpdateEquipmentForUID);
		WorldEventSystem->Widget.OnUpdateInventorySelectedCancel.AddDynamic(this, &UInventoryWidget::OnUpdateSelectedCancel);
	}

	InventoryTileView->OnItemClicked().AddUObject(this, &UInventoryWidget::OnItemClicked);
	//OwnedCharacterTileView->OnItemClicked().AddUObject(this, &UInventoryWidget::OnCharacterClicked);

	// 처음 생성될 인벤토리 세팅
	UpdateInventoryType(EUIType::PopupInventory);

}

void UInventoryWidget::NativeDestruct()
{
	if (WorldEventSystem)
	{
		WorldEventSystem->Widget.OnUpdateInventory.RemoveDynamic(this, &UInventoryWidget::OnUpdateInventory);
		WorldEventSystem->Widget.OnUpdateCharacterEquipment.RemoveDynamic(this, &UInventoryWidget::OnUpdateEquipmentForUID);
		WorldEventSystem->Widget.OnUpdateInventorySelectedCancel.RemoveDynamic(this, &UInventoryWidget::OnUpdateSelectedCancel);
	}

	Super::NativeDestruct();
}

void UInventoryWidget::OpenUI()
{
	UIType = EUIType::PopupInventory;

	Super::OpenUI();

	FilterReset();
	RefreshUIByMode();
}

void UInventoryWidget::CloseUI()
{
	Super::CloseUI();
}

void UInventoryWidget::UpdateInventoryType(EUIType InUIType)
{
#pragma region 유효성 검사
	if (InUIType != EUIType::PopupInventory &&
		InUIType != EUIType::PopupCharacterInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("잘못된 인벤토리 타입입니다"));
		return;
	}
#pragma endregion

	SelectedInventoryType = InUIType;

	RefreshUIByMode();
}

void UInventoryWidget::FilterReset()
{
	if (SelectedInventoryType == EUIType::PopupInventory)
	{
		CurrentItemGroup = EItemGroup::None;
		CurrentEquipGroup = EEquipGroup::None;

		CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::None);
		SortInventory();
	}
	else if (SelectedInventoryType == EUIType::PopupCharacterInfo)
	{
		CurrentItemGroup = EItemGroup::Equip;
		CurrentEquipGroup = EEquipGroup::None;

		CachedItemList = InventorySystem->GetFilteredInventoryList(EItemGroup::Equip);
		SortInventory();
	}
	else
		return;
}

void UInventoryWidget::FilterWeapon()
{
	CurrentItemGroup = EItemGroup::Equip;
	CurrentEquipGroup = EEquipGroup::Weapon;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup, CurrentEquipGroup);
	SortInventory();
}

void UInventoryWidget::FilterArmor()
{
	CurrentItemGroup = EItemGroup::Equip;
	CurrentEquipGroup = EEquipGroup::Armor;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup, CurrentEquipGroup);
	SortInventory();
}

void UInventoryWidget::FilterHelmet()
{
	CurrentItemGroup = EItemGroup::Equip;
	CurrentEquipGroup = EEquipGroup::Helmet;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup, CurrentEquipGroup);
	SortInventory();
}

void UInventoryWidget::FilterGrowth()
{
	CurrentItemGroup = EItemGroup::GrowthItem;
	CurrentEquipGroup = EEquipGroup::None;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup);
	SortInventory();
}

void UInventoryWidget::FilterGoods()
{
	CurrentItemGroup = EItemGroup::Goods;
	CurrentEquipGroup = EEquipGroup::None;

	CachedItemList = InventorySystem->GetFilteredInventoryList(CurrentItemGroup);
	SortInventory();
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
	if (EquipmentSlotWidget)
	{
		EquipmentSlotWidget->RefreshEquipment(InCharacterUID);
	}
}

void UInventoryWidget::OnUpdateSelectedCancel()
{
	InventoryTileView->ClearSelection();
}

void UInventoryWidget::OnItemClicked(UObject* InItemData)
{
	UItemData* ItemData = Cast<UItemData>(InItemData);

#pragma region 유효성 검사
	if (!ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventoryWidget(EquipmentTileItemClicked)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	ActionButtonWidget->SetupItem(ItemData);
	ActionButtonWidget->SetupCharacterUID(TempCharacterUID);		// 현재 캐릭터 목록 미구현으로 인해 임시 사용(선택한 캐릭터 입력)
	ActionButtonWidget->SetVisibleButton(SelectedInventoryType);
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

void UInventoryWidget::RefreshUIByMode()
{
	CurrentEquipGroup = EEquipGroup::None;

	if (SelectedInventoryType == EUIType::PopupInventory)
	{
		CurrentItemGroup = EItemGroup::None;
		
		Btn_FilterReset->SetVisibility(ESlateVisibility::Visible);

		Btn_FilterWeapon->SetVisibility(ESlateVisibility::Visible);
		Btn_FilterArmor->SetVisibility(ESlateVisibility::Visible);
		Btn_FilterHelmet->SetVisibility(ESlateVisibility::Visible);

		Btn_FilterGrowth->SetVisibility(ESlateVisibility::Visible);
		Btn_FilterGoods->SetVisibility(ESlateVisibility::Visible);

		EquipmentSlotWidget->SetVisibility(ESlateVisibility::Hidden);

		ActionButtonWidget->SetVisibility(ESlateVisibility::Hidden);

		FilterReset();
	}
	else if(SelectedInventoryType == EUIType::PopupCharacterInfo)
	{
		CurrentItemGroup = EItemGroup::Equip;

		Btn_FilterReset->SetVisibility(ESlateVisibility::Visible);

		Btn_FilterWeapon->SetVisibility(ESlateVisibility::Visible);
		Btn_FilterArmor->SetVisibility(ESlateVisibility::Visible);
		Btn_FilterHelmet->SetVisibility(ESlateVisibility::Visible);

		Btn_FilterGrowth->SetVisibility(ESlateVisibility::Hidden);
		Btn_FilterGoods->SetVisibility(ESlateVisibility::Hidden);

		EquipmentSlotWidget->SetVisibility(ESlateVisibility::Visible);
		EquipmentSlotWidget->RefreshEquipment(0);

		ActionButtonWidget->SetVisibility(ESlateVisibility::Hidden);

		FilterReset();
	}
}

void UInventoryWidget::SortInventory()
{
	Algo::Sort(CachedItemList, [](UItemData* ItemA, UItemData* ItemB) {
			if (!ItemA && !ItemB) return false;
			if (!ItemA) return false;
			if (!ItemB) return true;

			if (ItemA->GetItemGroup() != ItemB->GetItemGroup())
			{
				if (ItemA->GetItemGroup() == EItemGroup::None) return false;		// A의 ItemGroup이 None이면 뒤로
				if (ItemB->GetItemGroup() == EItemGroup::None) return true;			// B의 ItemGroup이 None이면 A를 앞으로

				return ItemA->GetItemGroup() < ItemB->GetItemGroup();				// ItemGroup Enum 순서대로 정렬
			}

			if (ItemA->GetEquipGroup() != ItemB->GetEquipGroup())
			{
				if (ItemA->GetEquipGroup() == EEquipGroup::None) return false;		// A의 EquipGroup이 None이면 뒤로
				if (ItemB->GetEquipGroup() == EEquipGroup::None) return true;		// B의 EquipGroup이 None이면 A를 앞으로

				return ItemA->GetEquipGroup() < ItemB->GetEquipGroup();				// EquipGroup Enum 순서대로 정렬
			}

			const auto TableDataA = ItemA->GetTableData();
			const auto TableDataB = ItemB->GetTableData();

			return TableDataA.DataId < TableDataB.DataId;							// A와 B의 ItemGroup과 EquipGroup이 같을 때 DataId 오름차순으로 정렬
		});

	if (InventoryTileView)
	{
		InventoryTileView->ClearSelection();
		InventoryTileView->SetListItems(CachedItemList);
	}
	if (ActionButtonWidget)
	{
		ActionButtonWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
