// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGoodsType : uint8
{
	None,
	Ticket,
	Gem,
	Gold,
};

UENUM(BlueprintType)
enum class EGradeType : uint8
{
	Common,
	Uncommon,
	Rare,
	Unique,
	Legend,
};
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,

	//무기
	OneHandSword,
	TwoHandSword,
	OneHandAxe,
	TwoHandAxe,
	Staff,				//양손
	Wand,				//한손
	Shield,
	Bow,				//양손, 화살통 없음

	Helmet,
	Armor,

	//성장 아이템
	BowMasteryPiece,		//활 업글 아이템
	SwordMasteryPiece,		//검 업글 아이템
	AxeMasteryPiece,
	StaffMasteryPiece,
	WandMasteryPiece,
	ShieldMasteryPiece,
};

UENUM(BlueprintType)
enum class EEquipType : uint8
{
	None,
	Sword,
	Axe,
	Staff,
	Wand,
	Shield,
	Bow,
	Armor,
	Helmet,
};

//아이템 필터용 그룹
UENUM(BlueprintType)
enum class EItemGroup : uint8
{
	None,
	Equip,
	GrowthItem,
};

UENUM(BlueprintType)
enum class EEquipGroup : uint8
{
	None,
	Weapon,
	Helmet,
	Armor,
};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	None,
	Damage,
	Heal,
	Splash,
};

UENUM(BlueprintType)
enum class ESkillApplyType : uint8
{
	Instant,
	PerSec,
};

UENUM(BlueprintType)
enum class ETargetingPriority : uint8
{
	Close,				//기본 가까운 적
	LowestDef,			//방어력 가장 낮은 적
	HighestDef,			//방어력 가장 높은 적
	Basement,			//기지
};

UENUM(BlueprintType)
enum class ETableDataType : uint8
{
	Valkyrie,
	Attack,
	Contents,
	Goods,
	Item,
	Reward,
	Skill,
	SkillEffect,
	Stage,
	StageInfo,
	StageReward,
	StatGroup,
	Unit,
	TopMenu,
	Projectile,
	UnitUpgradeStat,
	Targeting,
};

UENUM(BlueprintType)
enum class ESaveType : uint8
{
	CheckAccount,
	Account,
	Gacha,
	Goods,
	Item,
	Stage,
	UnitUpgrade,
	Valkyrie
};

UENUM(BlueprintType)
enum class EContentType : uint8
{
	None,
	HUD,
	Menu,
	Popup,
};
