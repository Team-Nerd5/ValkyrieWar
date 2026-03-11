// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/GameDataHelper.h"
#include "GameSystem/Instance/Game/DataManager.h"

EItemGroup UGameDataHelper::GetItemGroup(EItemType InItemType)
{
	switch (InItemType)
	{
	case EItemType::ShieldMasteryPiece:
	case EItemType::AxeMasteryPiece:
	case EItemType::BowMasteryPiece:
	case EItemType::StaffMasteryPiece:
	case EItemType::SwordMasteryPiece:
	case EItemType::WandMasteryPiece:
		return EItemGroup::GrowthItem;

	default:
		return EItemGroup::Equip;
	}
}

EEquipGroup UGameDataHelper::GetEquipGroup(EItemType InItemType)
{
	switch (InItemType)
	{
	case EItemType::Bow:
	case EItemType::OneHandSword:
	case EItemType::TwoHandSword:
	case EItemType::OneHandAxe:
	case EItemType::TwoHandAxe:
	case EItemType::Staff:
	case EItemType::Wand:
	case EItemType::Shield:
		return EEquipGroup::Weapon;
	case EItemType::Armor:
		return EEquipGroup::Armor;
	case EItemType::Helmet:
		return EEquipGroup::Helmet;

	default:
		return EEquipGroup::None;
	}
}

bool UGameDataHelper::GetTopMenuData(EUIType InUIType, UGameInstance* GameInstance, FTopMenuDataRow& OutTopMenuData)
{
	if (UDataManager* DataManager = GameInstance->GetSubsystem<UDataManager>())
	{
		int32 TopMenuId = DataManager->GetContentsModule()->GetTopMenuId(InUIType);
		if (TopMenuId > 0)
		{
			FTopMenuDataRow TopMenu = DataManager->GetTopMenuModule()->GetTopMenuTable(TopMenuId);
			if (TopMenu.DataId > 0)
			{
				OutTopMenuData = TopMenu;
				return true;
			}
		}
	}

	return false;
}
