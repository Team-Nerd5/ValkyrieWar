// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/OKM/TestPopupWidget.h"

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

void UTestPopupWidget::InitMenu(UItemData* ItemData, EUIType InUIType)
{
#pragma region 유효성 검사
	if (!ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[PopupWidget(InitMenu)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	CachedItemData = ItemData;

	if (InUIType == EUIType::PopupInventory)					// 인벤토리
	{
		Btn_Cancel->SetVisibility(ESlateVisibility::Visible);
		Btn_Sell->SetVisibility(ESlateVisibility::Visible);

		// 장비가 아닐 때만 사용 버튼 표시
		if (CachedItemData->GetItemGroup() != EItemGroup::Equip)
		{
			Btn_Use->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Btn_Use->SetVisibility(ESlateVisibility::Hidden);
		}

		Btn_Equip->SetVisibility(ESlateVisibility::Hidden);
		Btn_UnEquip->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (InUIType == EUIType::PopupCharacterInfo)			// 장비창 (Enum이 잘못 됬다면 추후 수정)
	{
		Btn_Cancel->SetVisibility(ESlateVisibility::Visible);

		Btn_Sell->SetVisibility(ESlateVisibility::Hidden);
		Btn_Use->SetVisibility(ESlateVisibility::Hidden);

		if (ItemData->GetEquipCharacter() == 0)
		{
			Btn_Equip->SetVisibility(ESlateVisibility::Visible);
			Btn_UnEquip->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			Btn_Equip->SetVisibility(ESlateVisibility::Hidden);
			Btn_UnEquip->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
		return;
}

void UTestPopupWidget::PopupSetHidden()
{
	if(Btn_Sell->GetVisibility() == ESlateVisibility::Visible)
		Btn_Sell->SetVisibility(ESlateVisibility::Hidden);
	if (Btn_Use->GetVisibility() == ESlateVisibility::Visible)
		Btn_Use->SetVisibility(ESlateVisibility::Hidden);
	if (Btn_Equip->GetVisibility() == ESlateVisibility::Visible)
		Btn_Equip->SetVisibility(ESlateVisibility::Hidden);
	if (Btn_UnEquip->GetVisibility() == ESlateVisibility::Visible)
		Btn_UnEquip->SetVisibility(ESlateVisibility::Hidden);
	if (Btn_Cancel->GetVisibility() == ESlateVisibility::Visible)
		Btn_Cancel->SetVisibility(ESlateVisibility::Hidden);
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

	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	// 판매 아이템이 장착아이템이라면 장착해제
	if(!(CachedItemData->GetEquipGroup() == EEquipGroup::None))
		WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast(0, CachedItemData->GetEquipGroup());
	CachedItemData = nullptr;

	PopupSetHidden();
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

	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	CachedItemData = nullptr;

	PopupSetHidden();
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

	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast(TempCharacterUID, CachedItemData->GetEquipGroup());

	PopupSetHidden();
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

	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast(0, CachedItemData->GetEquipGroup());

	PopupSetHidden();
}

