// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/ItemListWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Widget/Item/Tools/Tab/TabMenuWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"

#include "Algo/Sort.h"

void UItemListWidget::SetMenu(TMap<EInventoryFilterType, FString> InMenuNameData)
{
	TabType = ETabType::Inventory;

	if (TabMenu)
	{
		for (auto Data : InMenuNameData)
		{
			int32 FilterIndex = static_cast<int32>(Data.Key);
			FString TabName = Data.Value;
			TabMenu->AddTab(TabType, FilterIndex, TabName);
		}
	}

	UpdateButton();
}

void UItemListWidget::SetMenu(TMap<ECharacterInfoFilterType, FString> InMenuNameData)
{
	TabType = ETabType::CharacterInfo;

	if (TabMenu)
	{
		for (auto Data : InMenuNameData)
		{
			int32 FilterIndex = static_cast<int32>(Data.Key);
			FString TabName = Data.Value;
			TabMenu->AddTab(TabType, FilterIndex, TabName);
		}
	}

	UpdateButton();
}

void UItemListWidget::SetData(TArray<class UItemData*> InItemList)
{
	CachedItemList = InItemList;

	UpdateFilteredItemList();
}

void UItemListWidget::InitFilterIndex(int32 InIndex)
{
	SelectedFilterIndex = InIndex;

	UpdateFilteredItemList();

	if (TabMenu)
	{
		TabMenu->SetTab(InIndex);
	}
}

void UItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UItemListWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UItemListWidget::OnItemSelected(UObject* InItemData)
{
	//아이템 선택 시 버튼 
}

void UItemListWidget::UpdateFilteredItemList()
{
	FilteredItemList.Empty();

	if (TabType == ETabType::Inventory)
	{
		EInventoryFilterType FilterType = static_cast<EInventoryFilterType>(SelectedFilterIndex);

		switch (FilterType)
		{
			case EInventoryFilterType::All:
				FilteredItemList = CachedItemList;
				break;
			case EInventoryFilterType::Equipment:
				for(UItemData* Item : CachedItemList)
				{
					if (Item && Item->GetItemGroup() == EItemGroup::Equip)
					{
						FilteredItemList.Add(Item);
					}
				}
				break;
			case EInventoryFilterType::Item:
				for (UItemData* Item : CachedItemList)
				{
					if (Item && Item->GetItemGroup() == EItemGroup::GrowthItem)
					{
						FilteredItemList.Add(Item);
					}
				}
				break;
		}
	}
	else
	{
		ECharacterInfoFilterType FilterType = static_cast<ECharacterInfoFilterType>(SelectedFilterIndex);

		switch (FilterType)
		{
		case ECharacterInfoFilterType::All:
			FilteredItemList = CachedItemList;
			break;
		case ECharacterInfoFilterType::Weapon:
			for (UItemData* Item : CachedItemList)
			{
				if (Item && Item->GetEquipGroup() == EEquipGroup::Weapon)
				{
					FilteredItemList.Add(Item);
				}
			}
			break;
		case ECharacterInfoFilterType::Armor:
			for (UItemData* Item : CachedItemList)
			{
				if (Item && Item->GetEquipGroup() == EEquipGroup::Armor)
				{
					FilteredItemList.Add(Item);
				}
			}
			break;
		case ECharacterInfoFilterType::Helmet:
			for (UItemData* Item : CachedItemList)
			{
				if (Item && Item->GetEquipGroup() == EEquipGroup::Helmet)
				{
					FilteredItemList.Add(Item);
				}
			}
			break;
		}
	}

	if (InventoryTileView)
	{
		SortInventory();

		InventoryTileView->ClearSelection();
		InventoryTileView->SetListItems(FilteredItemList);

	}
}

void UItemListWidget::UpdateButton()
{
	if (SellButton)
	{
		SellButton->SetVisibility(TabType == ETabType::Inventory ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (EquipButton)
	{
		EquipButton->SetVisibility(TabType == ETabType::CharacterInfo ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UItemListWidget::SortInventory()
{
	Algo::Sort(FilteredItemList, [](UItemData* ItemA, UItemData* ItemB) {
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

}
