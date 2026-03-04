// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/ActionButtonWidget.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void UActionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
	}
	WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this);

	if (Btn_Sell)
		Btn_Sell->OnClicked.AddDynamic(this, &UActionButtonWidget::Sell);
	if (Btn_Equip)
		Btn_Equip->OnClicked.AddDynamic(this, &UActionButtonWidget::Equip);
	if (Btn_Unequip)
		Btn_Unequip->OnClicked.AddDynamic(this, &UActionButtonWidget::Unequip);
	if (Btn_Cancel)
		Btn_Cancel->OnClicked.AddDynamic(this, &UActionButtonWidget::SelectCancel);
}

void UActionButtonWidget::SetVisibleButton(EUIType InCurrentUIType)
{
	if (InCurrentUIType == EUIType::PopupInventory)
	{
		Btn_Sell->SetVisibility(ESlateVisibility::Visible);

		Btn_Equip->SetVisibility(ESlateVisibility::Hidden);
		Btn_Unequip->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (InCurrentUIType == EUIType::PopupCharacterInfo)
	{
		Btn_Sell->SetVisibility(ESlateVisibility::Hidden);

		if (CachedItemData->GetEquipCharacter() == 0)
		{
			Btn_Equip->SetVisibility(ESlateVisibility::Visible);
			Btn_Unequip->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			Btn_Equip->SetVisibility(ESlateVisibility::Hidden);
			Btn_Unequip->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
		return;
}
void UActionButtonWidget::SetupItem(UItemData* InItemData)
{
#pragma region 유효성 검사
	if (!InItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(SetupItem)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	CachedItemData = InItemData;

	SetVisibility(ESlateVisibility::Visible);
}

void UActionButtonWidget::SetupCharacterUID(uint64 InCharacterUID)
{
#pragma region 유효성 검사
	if (InCharacterUID == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(SetupCharacterUID)] 선택된 캐릭터UID가 없습니다"));
		return;
	}
#pragma endregion
	CachedCharacterUID = InCharacterUID;
}


void UActionButtonWidget::Sell()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(Sell)] InventorySystem이 없습니다"));
		return;
	}
	if (!WorldEventSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(Sell)] WorldEventSystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(Sell)] 선택된 아이템이 없습니다"));
		return;
	}
#pragma endregion

	// 개수 입력 필요시 추가
	InventorySystem->SellItem(CachedItemData, 1);

	if (!(CachedItemData->GetEquipGroup() == EEquipGroup::None))
	{
		// 판매 아이템이 장착아이템이라면 장착해제
		uint64 TempCharacterUID = CachedItemData->GetEquipCharacter();
		InventorySystem->UnEquipItem(CachedItemData);
		// 인벤토리 장비칸 갱신
		WorldEventSystem->Widget.OnUpdateCharacterEquipment.Broadcast(TempCharacterUID);
	}

	// 양의 변화가 필요한 아이템인 경우 개수 갱신
	if (CachedItemData->GetItemGroup() == EItemGroup::Goods ||
		CachedItemData->GetItemGroup() == EItemGroup::GrowthItem)
	{
		WorldEventSystem->Widget.OnUpdateInventoryAmountChanged.Broadcast();
	}
	// 인벤토리 갱신
	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	// 인벤토리 선택 해제
	WorldEventSystem->Widget.OnUpdateInventorySelectedCancel.Broadcast();

	CachedItemData = nullptr;

	SetVisibility(ESlateVisibility::Hidden);
}

void UActionButtonWidget::Equip()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(Equip)] InventorySystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(Equip)] 선택퇸 아이템이 없습니다"));
		return;
	}
	if (CachedCharacterUID == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(Equip)] 선택된 캐릭터UID가 없습니다"));
		return;
	}
#pragma endregion

	InventorySystem->EquipItem(CachedItemData, CachedCharacterUID);

	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	WorldEventSystem->Widget.OnUpdateCharacterEquipment.Broadcast(CachedCharacterUID);
	WorldEventSystem->Widget.OnUpdateInventorySelectedCancel.Broadcast();

	CachedItemData = nullptr;
	CachedCharacterUID = 0;

	SetVisibility(ESlateVisibility::Hidden);
}

void UActionButtonWidget::Unequip()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(UnEquip)] InventorySystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(UnEquip)] 선택퇸 아이템이 없습니다"));
		return;
	}
	if (CachedCharacterUID == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[ButtonWidget(UnEquip)] 선택된 캐릭터UID가 없습니다"));
		return;
	}
#pragma endregion
	uint64 TempCharacterUID = CachedItemData->GetEquipCharacter();

	InventorySystem->UnEquipItem(CachedItemData);

	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	WorldEventSystem->Widget.OnUpdateCharacterEquipment.Broadcast(TempCharacterUID);
	WorldEventSystem->Widget.OnUpdateInventorySelectedCancel.Broadcast();

	CachedItemData = nullptr;

	SetVisibility(ESlateVisibility::Hidden);
}

void UActionButtonWidget::SelectCancel()
{
	WorldEventSystem->Widget.OnUpdateInventorySelectedCancel.Broadcast();

	SetVisibility(ESlateVisibility::Hidden);
}
