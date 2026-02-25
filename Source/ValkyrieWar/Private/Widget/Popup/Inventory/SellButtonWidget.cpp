// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Inventory/SellButtonWidget.h"

void USellButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
		WorldEventSystem = World->GetSubsystem<UWorldEventSystem>();
	}

	if (Btn_Sell)
		Btn_Sell->OnClicked.AddDynamic(this, &USellButtonWidget::Sell);
	if (Btn_Cancel)
		Btn_Cancel->OnClicked.AddDynamic(this, &USellButtonWidget::CloseUI);

	CloseUI();
}

void USellButtonWidget::OpenUI()
{
	Super::OpenUI();
}

void USellButtonWidget::CloseUI()
{
	Super::CloseUI();
}

void USellButtonWidget::SetupSellItem(UItemData* InItemData)
{
#pragma region 유효성 검사
	if (!InItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[SellButtonWidget(SetupSellItem)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	CachedItemData = InItemData;
	OpenUI();
}

void USellButtonWidget::Sell()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[SellButtonWidget(Sell)] InventorySystem이 없습니다"));
		return;
	}
	if (!WorldEventSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[SellButtonWidget(Sell)] WorldEventSystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[SellButtonWidget(Sell)] 선택퇸 아이템이 없습니다"));
		return;
	}
#pragma endregion
	// 개수 입력 필요시 추가
	InventorySystem->SellItem(CachedItemData, 1);

	WorldEventSystem->Widget.OnUpdateInventory.Broadcast();
	// 판매 아이템이 장착아이템이라면 장착해제
	if (!(CachedItemData->GetEquipGroup() == EEquipGroup::None))
		WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast(0, CachedItemData->GetEquipGroup());
	CachedItemData = nullptr;

	CloseUI();
}
