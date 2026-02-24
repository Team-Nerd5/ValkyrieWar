// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/GameDataHelper.h"

EItemGroup UGameDataHelper::GetItemGroup(EItemType InItemType)
{
	switch (InItemType)
	{
	case EItemType::Gold:
	case EItemType::Gem:
	case EItemType::Ticket:
		return EItemGroup::Goods;

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
