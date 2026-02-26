// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/EquipButtonWidget.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void UEquipButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>();
	}
	WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this);

	if (Btn_Equip)
		Btn_Equip->OnClicked.AddDynamic(this, &UEquipButtonWidget::Equip);
	if (Btn_Unequip)
		Btn_Unequip->OnClicked.AddDynamic(this, &UEquipButtonWidget::Unequip);
	if (Btn_Cancel)
		Btn_Cancel->OnClicked.AddDynamic(this, &UEquipButtonWidget::CloseUI);

	CloseUI();
}

void UEquipButtonWidget::OpenUI()
{
	Super::OpenUI();
}

void UEquipButtonWidget::CloseUI()
{
	Super::CloseUI();
}

void UEquipButtonWidget::SetupEquipItem(UItemData* InItemData)
{
#pragma region 유효성 검사
	if (!InItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[EquipButtonWidget(SetupEquipItem)] ItemData가 없습니다"));
		return;
	}
#pragma endregion

	CachedItemData = InItemData;
	OpenUI();

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

void UEquipButtonWidget::Equip()
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

	// 추후 선택한 케릭터UID 입력 필요
	
	InventorySystem->EquipItem(CachedItemData, TempCharacterUID);

	WorldEventSystem->Widget.OnUpdateEquipment.Broadcast();
	WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast(TempCharacterUID);

	CachedItemData = nullptr;

	CloseUI();
}
void UEquipButtonWidget::Unequip()
{
#pragma region 유효성 검사
	if (!InventorySystem)
	{
		UE_LOG(LogTemp, Log, TEXT("[EquipButtonWidget(UnEquip)] InventorySystem이 없습니다"));
		return;
	}
	if (!CachedItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("[EquipButtonWidget(UnEquip)] 선택퇸 아이템이 없습니다"));
		return;
	}
#pragma endregion

	InventorySystem->UnEquipItem(CachedItemData);

	WorldEventSystem->Widget.OnUpdateEquipment.Broadcast();
	WorldEventSystem->Widget.OnChangeEquipCharacter.Broadcast(TempCharacterUID);

	CachedItemData = nullptr;

	CloseUI();
}
