// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/ItemListWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Widget/Item/Tools/Tab/TabMenuWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"

void UItemListWidget::SetMenu(TMap<EInventoryFilterType, FString> InMenuNameData)
{
	TabType = ETabType::Inventory;

	if (TabMenuClass)
	{
		UTabMenuWidget* TabMenu = CreateWidget<UTabMenuWidget>(this, TabMenuClass);

		if (TabMenu)
		{
			if (TabMenuContainer)
			{
				TabMenuContainer->AddChild(TabMenu);
			}

			for (auto Data : InMenuNameData)
			{
				int32 FilterIndex = static_cast<int32>(Data.Key);
				FString TabName = Data.Value;
				TabMenu->AddTab(TabType, FilterIndex, TabName);
			}
		}
	}

	UpdateButton();
}

void UItemListWidget::SetMenu(TMap<ECharacterInfoFilterType, FString> InMenuNameData)
{
	TabType = ETabType::CharacterInfo;

	if (TabMenuClass)
	{
		UTabMenuWidget* TabMenu = CreateWidget<UTabMenuWidget>(this, TabMenuClass);

		if (TabMenu)
		{
			if (TabMenuContainer)
			{
				TabMenuContainer->AddChild(TabMenu);
			}

			for (auto Data : InMenuNameData)
			{
				int32 FilterIndex = static_cast<int32>(Data.Key);
				FString TabName = Data.Value;
				TabMenu->AddTab(TabType, FilterIndex, TabName);
			}
		}
	}

	UpdateButton();
}

void UItemListWidget::SetData(TArray<class UItemData*> InItemList)
{
	CachedItemList = InItemList;

	UpdateFilteredItemList();
}

void UItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Widget.OnTabMenuSelected.AddDynamic(this, &UItemListWidget::OnTabMenuChanged);
	}
}

void UItemListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Widget.OnTabMenuSelected.RemoveDynamic(this, &UItemListWidget::OnTabMenuChanged);
	}
}

//탭이 눌림
void UItemListWidget::OnTabMenuChanged(int32 InSelectedTab)
{
	SelectedFilterIndex = InSelectedTab;

	UpdateFilteredItemList();
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
