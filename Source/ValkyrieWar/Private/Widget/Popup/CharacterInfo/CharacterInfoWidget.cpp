// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/CharacterInfoWidget.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "Widget/HUD/LobbyWidget.h"
#include "Widget/Popup/Inventory/ItemListWidget.h"
#include "Widget/Popup/CharacterInfo/ValkyrieListWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

void UCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.AddUniqueDynamic(this, &UCharacterInfoWidget::OnInventoryUpdate);
		EventSystem->Widget.OnTabMenuSelected.AddUniqueDynamic(this, &UCharacterInfoWidget::OnTabMenuChanged);
		EventSystem->Widget.OnValkyrieSelected.AddUniqueDynamic(this, &UCharacterInfoWidget::OnValkyrieSelected);
	}

	if (SetMainButton)
		SetMainButton->OnClicked.AddUniqueDynamic(this, &UCharacterInfoWidget::OnClickSetMain);
}

void UCharacterInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (EventSystem)
	{
		EventSystem->Widget.OnUpdateInventory.RemoveDynamic(this, &UCharacterInfoWidget::OnInventoryUpdate);
		EventSystem->Widget.OnTabMenuSelected.RemoveDynamic(this, &UCharacterInfoWidget::OnTabMenuChanged);
		EventSystem->Widget.OnValkyrieSelected.RemoveDynamic(this, &UCharacterInfoWidget::OnValkyrieSelected);
	}
	if (SetMainButton)
		SetMainButton->OnClicked.RemoveDynamic(this, &UCharacterInfoWidget::OnClickSetMain);
}

void UCharacterInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CurrentFilterType = ECharacterInfoFilterType::All;

	if (ItemListWidget)
	{
		ItemListWidget->SetMenu(CharacterInfoTabNameData);
	}
	InitItemList();
	InitValkyrieList();
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

	if(WeaponItem && TypeIcons.Contains(EEquipGroup::Weapon))
		WeaponItem->InitEquip(TypeIcons.FindChecked(EEquipGroup::Weapon));
	if(ArmorItem && TypeIcons.Contains(EEquipGroup::Armor))
		ArmorItem->InitEquip(TypeIcons.FindChecked(EEquipGroup::Armor));
	if(HelmetItem && TypeIcons.Contains(EEquipGroup::Helmet))
		HelmetItem->InitEquip(TypeIcons.FindChecked(EEquipGroup::Helmet));
}

void UCharacterInfoWidget::CloseUI()
{
	//로비 켜줘야함
	if (EventSystem)
	{
		EventSystem->Widget.OnCharacterInfoWidgetClosed.Broadcast();
		//로비캐릭터를 메인으로 바꿔줘야함
		if (UGameManager* GameManager = GetGameInstance<UGameManager>())
		{
			EventSystem->Widget.OnValkyrieSelected.Broadcast(GameManager->GetSelectedValkyrie());
		}
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
void UCharacterInfoWidget::InitValkyrieList()
{
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		OriginValkyires = DataManager->GetValkyrieModule()->GetValkyrieList();
	}

	if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
	{
		MainValkyrie = GameManager->GetSelectedValkyrie();
	}
	if (ValkyrieListWidget)
	{
		ValkyrieListWidget->SetData(OriginValkyires);
		ValkyrieListWidget->SelectValkyrie(MainValkyrie);
		OnValkyrieSelected(MainValkyrie);
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

void UCharacterInfoWidget::OnClickSetMain()
{
	if (SelectedValkyrie)
	{
		//얘를 메인으로
		if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
		{
			GameManager->SelectVakyrie(SelectedValkyrie->GetUID());
		}

		MainValkyrie = SelectedValkyrie;
	}

	//메인 관련 버튼, UI 갱신
}

//선택된 캐릭터가 바뀜
void UCharacterInfoWidget::OnValkyrieSelected(UValkyrieData* InValkyrieData)
{
	SelectedValkyrie = InValkyrieData;

	if (UItemData* EquipWeapon = SelectedValkyrie->GetEquippedItem(EEquipGroup::Weapon))
	{
		WeaponItem->SetEquip(EquipWeapon->GetIcon());
	}
	if (UItemData* EquipArmor = SelectedValkyrie->GetEquippedItem(EEquipGroup::Armor))
	{
		ArmorItem->SetEquip(EquipArmor->GetIcon());
	}
	if (UItemData* EquipHelmet = SelectedValkyrie->GetEquippedItem(EEquipGroup::Helmet))
	{
		HelmetItem->SetEquip(EquipHelmet->GetIcon());
	}

	if (SetMainButton)
	{
		SetMainButton->SetIsEnabled(SelectedValkyrie != MainValkyrie);
	}

	if (MainValkyrieIcon)
	{
		MainValkyrieIcon->SetVisibility(SelectedValkyrie == MainValkyrie ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
