// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/ItemInfo/ItemInfoWidget.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/ItemModule.h"
#include "Data/Module/ValkyrieModule.h"
#include "Data/Game/ItemData.h"

void UItemInfoWidget::Init(int32 InItemUID)
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
		break;
	case EItemGroup::GrowthItem:
		break;
	default:
		break;
	}
}
