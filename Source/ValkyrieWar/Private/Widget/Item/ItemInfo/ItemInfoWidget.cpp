// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/ItemInfo/ItemInfoWidget.h"

#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/ValkyrieModule.h"
#include "Data/Game/ItemData.h"

#include "Components/CanvasPanelSlot.h"

#include "Blueprint/WidgetLayoutLibrary.h"

void UItemInfoWidget::Init(int32 InItemUID, UWidget* ContextWidget)
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UItemModule* ItemModule = DataManager->GetItemModule();
	if (!ItemModule)
		return;
	UItemData* ItemData = ItemModule->GetItem(InItemUID);
	if (!ItemData)
		return;

	EquipItemInfo(ItemData);
	SetvisibleInfo(ItemData->GetItemGroup());
	SetPosition(ItemData->GetUID(), ContextWidget);
}

void UItemInfoWidget::EquipItemInfo(UItemData* InItemData)
{
	if (ItemName)
		ItemName->SetText(FText::FromString(InItemData->GetTableData().Name));
	if (ItemSellPrice)
		ItemSellPrice->SetText(FText::AsNumber(InItemData->GetTableData().SellPrice));
	if (ItemAmount)
		ItemAmount->SetText(FText::AsNumber(InItemData->GetAmount()));

	// 장비일 경우
	if (InItemData->GetItemGroup() == EItemGroup::Equip)
	{
		UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
		if (!DataManager)
			return;
		UValkyrieModule* ValkyrieModule = DataManager->GetValkyrieModule();
		if (!ValkyrieModule)
			return;

		FString EquipGroupString = StaticEnum<EEquipGroup>()->GetNameStringByValue((int64)InItemData->GetEquipGroup());
		if (ItemEquipGroup)
			ItemEquipGroup->SetText(FText::FromString(EquipGroupString));
		if (ItemEquippedCharacter)
		{
			if (InItemData->GetEquipCharacter() != 0)
			{
				if (UValkyrieData* ValkyrieData = ValkyrieModule->GetExistValkyrie(InItemData->GetEquipCharacter()))
					ItemEquippedCharacter->SetText(FText::FromString(ValkyrieData->GetValkyrieName()));
				else
					ItemEquippedCharacter->SetText(FText::FromString(FString::Printf(TEXT("장착 가능"))));
			}
			else
			{
				ItemEquippedCharacter->SetText(FText::FromString(FString::Printf(TEXT("장착 가능"))));
			}
		}

		if (ItemStat_Health)
			ItemStat_Health->SetText(FText::AsNumber(InItemData->GetStat().Health));
		if (ItemStat_Attack)
			ItemStat_Attack->SetText(FText::AsNumber(InItemData->GetStat().Attack));
		if (ItemStat_Defence)
			ItemStat_Defence->SetText(FText::AsNumber(InItemData->GetStat().Defence));
	}
}

void UItemInfoWidget::SetvisibleInfo(EItemGroup InItemGroup)
{
	// TODO: 아이템 그룹에따라 위젯 설정
	switch (InItemGroup)
	{
	case EItemGroup::Equip:
		if (EquippedCharacterBox)
			EquippedCharacterBox->SetVisibility(ESlateVisibility::Visible);
		if (EquipGroupBox)
			EquipGroupBox->SetVisibility(ESlateVisibility::Visible);
		if (HealthBox)
			HealthBox->SetVisibility(ESlateVisibility::Visible);
		if (AttackBox)
			AttackBox->SetVisibility(ESlateVisibility::Visible);
		if (DefenceBox)
			DefenceBox->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemGroup::GrowthItem:
		if (EquippedCharacterBox)
			EquippedCharacterBox->SetVisibility(ESlateVisibility::Hidden);
		if (EquipGroupBox)
			EquipGroupBox->SetVisibility(ESlateVisibility::Hidden);
		if (HealthBox)
			HealthBox->SetVisibility(ESlateVisibility::Hidden);
		if (AttackBox)
			AttackBox->SetVisibility(ESlateVisibility::Hidden);
		if (DefenceBox)
			DefenceBox->SetVisibility(ESlateVisibility::Hidden);
		break;
	default:
		break;
	}
}

void UItemInfoWidget::SetPosition(int32 InItemUID, UWidget* ContextWidget)
{
	if (!ContextWidget)
		return;

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		CanvasSlot->SetAlignment(FVector2D(1.0f, 0.0f));

		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(ContextWidget);

		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(ContextWidget);
		float DPIScale = UWidgetLayoutLibrary::GetViewportScale(ContextWidget);
		FVector2D ScaleViewportSize = ViewportSize / DPIScale;
		FVector2D WidgetSize = GetDesiredSize();

		float ClampX = FMath::Clamp(MousePosition.X, WidgetSize.X, ScaleViewportSize.X);
		float ClampY = FMath::Clamp(MousePosition.Y, 0.0f, ScaleViewportSize.Y - WidgetSize.Y);
		FVector2D TouchPosition(ClampX - 30.0f, ClampY);

		CanvasSlot->SetPosition(TouchPosition);
	}
}
