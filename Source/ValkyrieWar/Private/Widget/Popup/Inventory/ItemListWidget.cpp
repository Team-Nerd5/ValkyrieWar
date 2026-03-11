// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/ItemListWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Widget/Item/Tools/Tab/TabMenuWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

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

	if (SellButton)
	{
		SellButton->OnClicked.AddDynamic(this, &UItemListWidget::OnClickSellItem);
		SellButton->SetIsEnabled(false);
	}

	if (AmountMinusButton)
	{
		AmountMinusButton->OnClicked.AddDynamic(this, &UItemListWidget::OnClickAmountMinusButton);
	}
	if (AmountPlusButton)
	{
		AmountPlusButton->OnClicked.AddDynamic(this, &UItemListWidget::OnClickAmountPlusButton);
	}
	if (SellAmountEditBox)
	{
		SellAmountEditBox->OnTextChanged.AddDynamic(this, &UItemListWidget::OnSellAmountChanged);
	}

	UpdateButton();
}

void UItemListWidget::SetMenu(TMap<ECharacterInfoFilterType, FString> InMenuNameData)
{
	TabType = ETabType::CharacterInfo;

	if (EquipButton)
	{
		EquipButton->OnClicked.AddDynamic(this, &UItemListWidget::OnClickEquipItem);
		EquipButton->SetIsEnabled(false);
	}

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
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnInventoryItemSelected.AddDynamic(this, &UItemListWidget::OnItemSelected);
	}
}

void UItemListWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnInventoryItemSelected.RemoveDynamic(this, &UItemListWidget::OnItemSelected);
	}
}

void UItemListWidget::OnClickSellItem()
{
	//판매 게이지 필요...
	if (SelectedItem)
	{
		//판매 확인 팝업
		//우선은 판매...

		if (UInventorySystem* InventorySystem = GetGameInstance()->GetSubsystem<UInventorySystem>())
		{
			InventorySystem->SellItem(SelectedItem, CurrentAmount);

			RefreshInventory();
		}
	}
}

void UItemListWidget::OnClickEquipItem()
{
	if (SelectedItem)
	{
		//장착 이벤트 호출
		//캐릭터 정보 위젯에서 현재 캐릭터 아이디 가져와서 세팅
		
	}
}

void UItemListWidget::OnClickAmountMinusButton()
{
	if (CurrentAmount > 1)
		CurrentAmount--;

	if (SellPriceText)
	{
		int32 Price = SelectedItem ? SelectedItem->GetTableData().SellPrice : 0;
		SellPriceText->SetText(FText::AsNumber(CurrentAmount * Price));
	}

	if (SellAmountEditBox)
		SellAmountEditBox->SetText(FText::AsNumber(CurrentAmount));
}

void UItemListWidget::OnClickAmountPlusButton()
{
	if (CurrentAmount < MaxAmount)
		CurrentAmount++;

	if (SellPriceText)
	{
		int32 Price = SelectedItem ? SelectedItem->GetTableData().SellPrice : 0;
		SellPriceText->SetText(FText::AsNumber(CurrentAmount * Price));
	}

	if (SellAmountEditBox)
		SellAmountEditBox->SetText(FText::AsNumber(CurrentAmount));
}

void UItemListWidget::OnSellAmountChanged(const FText& InText)
{
	int32 InAmount = FCString::Atoi(*InText.ToString());

	if (InAmount < 1)
		CurrentAmount = 1;

	if (InAmount > MaxAmount)
		CurrentAmount = MaxAmount;

	if (SellAmountEditBox)
		SellAmountEditBox->SetText(FText::AsNumber(CurrentAmount));
}

void UItemListWidget::RefreshInventory()
{
	bool IsSelectedItemExist = SelectedItem && SelectedItem->GetAmount() > 0;

	MaxAmount = IsSelectedItemExist ? SelectedItem->GetAmount() : 0;

	CurrentAmount = IsSelectedItemExist ? 1 : 0;

	if (SellAmountEditBox)
	{
		SellAmountEditBox->SetText(FText::AsNumber(CurrentAmount));
	}

	if (SellPriceText)
	{
		int32 Price = SelectedItem ? SelectedItem->GetTableData().SellPrice : 0;
		SellPriceText->SetText(FText::AsNumber(CurrentAmount * Price));
	}

	if (SellButton)
	{
		SellButton->SetIsEnabled(IsSelectedItemExist);
	}
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

void UItemListWidget::OnItemSelected(UItemData* InItemData)
{
	UButton* ActiveButton = (TabType == ETabType::Inventory) ? SellButton : EquipButton;

	if (SelectedItem == InItemData)
	{
		return;
	}

	ActiveButton->SetIsEnabled(InItemData != nullptr);

	SelectedItem = InItemData;

	if (SellAmountPanel)
	{
		SellAmountPanel->SetVisibility((TabType == ETabType::Inventory) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (GoldPanel)
	{
		GoldPanel->SetVisibility((TabType == ETabType::Inventory) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (TabType == ETabType::Inventory)
	{
		RefreshInventory();
	}
}
