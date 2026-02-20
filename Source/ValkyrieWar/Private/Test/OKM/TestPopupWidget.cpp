// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestPopupWidget.h"
#include "Data/Enum/DataEnums.h"

void UTestPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
		WorldEventSystem = World->GetSubsystem<UWorldEventSystem>();
	}

	Btn_Sell->SetVisibility(ESlateVisibility::Collapsed);
	Btn_Cancel->SetVisibility(ESlateVisibility::Collapsed);
	Btn_Use->SetVisibility(ESlateVisibility::Collapsed);
	Btn_Equip->SetVisibility(ESlateVisibility::Collapsed);
	Btn_UnEquip->SetVisibility(ESlateVisibility::Collapsed);

	if(Btn_Sell)
		Btn_Sell->OnClicked.AddDynamic(this, &UTestPopupWidget::Sell);
	if(Btn_Cancel)
		Btn_Cancel->OnClicked.AddDynamic(this, &UTestPopupWidget::CloseUI);
	if(Btn_Use)
		Btn_Use->OnClicked.AddDynamic(this, &UTestPopupWidget::Use);
	if(Btn_Equip)
		Btn_Equip->OnClicked.AddDynamic(this, &UTestPopupWidget::Equip);
	if(Btn_UnEquip)
		Btn_UnEquip->OnClicked.AddDynamic(this, &UTestPopupWidget::UnEquip);
	
}

void UTestPopupWidget::OpenUI()
{
	Super::OpenUI();
}

void UTestPopupWidget::CloseUI()
{
	Super::CloseUI();
}

void UTestPopupWidget::InitMenu(UItemData* ItemData)
{
#pragma region 유효성 검사
	if (!ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[PopupWidget(InitMenu)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	CachedItemData = ItemData;

	Btn_Sell->SetVisibility(ESlateVisibility::Visible);
	Btn_Cancel->SetVisibility(ESlateVisibility::Visible);

	if (ItemData->GetItemGroup() == EItemGroup::Weapon ||
		ItemData->GetItemGroup() == EItemGroup::Helmet ||
		ItemData->GetItemGroup() == EItemGroup::Armor)
	{
		if (ItemData->GetEquipCharacter() == 0)
		{
			Btn_Equip->SetVisibility(ESlateVisibility::Visible);
			Btn_UnEquip->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Btn_Equip->SetVisibility(ESlateVisibility::Collapsed);
			Btn_UnEquip->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		Btn_Use->SetVisibility(ESlateVisibility::Visible);
		Btn_Equip->SetVisibility(ESlateVisibility::Collapsed);
		Btn_UnEquip->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTestPopupWidget::Sell()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Sell)] InventorySystem이 없습니다"));
		return;
	}
	if (!WorldEventSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Sell)] WorldEventSystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Sell)] 선택퇸 아이템이 없습니다"));
		return;
	}
#pragma endregion

	// 개수 입력 필요시 추가
	InventorySystem->SellItem(CachedItemData, 1);

	WorldEventSystem->Widget.OnChangeItemAmount.Broadcast();
	CachedItemData = nullptr;
}

void UTestPopupWidget::Use()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Use)] InventorySystem이 없습니다"));
		return;
	}
	if (!WorldEventSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Use)] WorldEventSystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Use)] 선택퇸 아이템이 없습니다"));
		return;
	}
#pragma endregion

	// 개수 입력 필요시 추가
	InventorySystem->UseItem(CachedItemData, 1);

	Btn_Use->SetVisibility(ESlateVisibility::Collapsed);
	Btn_Equip->SetVisibility(ESlateVisibility::Collapsed);
	Btn_UnEquip->SetVisibility(ESlateVisibility::Collapsed);

	WorldEventSystem->Widget.OnChangeItemAmount.Broadcast();
	CachedItemData = nullptr;
}

void UTestPopupWidget::Equip()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Equip)] InventorySystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(Equip)] 선택퇸 아이템이 없습니다"));
		return;
	}
#pragma endregion

	// 추후 케릭터UID 입력 필요

	const uint64 TempCharacterUID = 1001001;
	InventorySystem->EquipItem(CachedItemData, TempCharacterUID);

	Btn_Use->SetVisibility(ESlateVisibility::Collapsed);
	Btn_Equip->SetVisibility(ESlateVisibility::Collapsed);
	Btn_UnEquip->SetVisibility(ESlateVisibility::Collapsed);

	WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast();
}

void UTestPopupWidget::UnEquip()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(UnEquip)] InventorySystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[TestPopupWidget(UnEquip)] 선택퇸 아이템이 없습니다"));
		return;
	}
#pragma endregion

	InventorySystem->UnEquipItem(CachedItemData);

	Btn_Use->SetVisibility(ESlateVisibility::Collapsed);
	Btn_Equip->SetVisibility(ESlateVisibility::Collapsed);
	Btn_UnEquip->SetVisibility(ESlateVisibility::Collapsed);

	WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast();
}
