// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGoodsType : uint8
{
	Ticket,
	Gem,
	Gold,
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Goods,
	Equip,
	Item,
};

//아이템 필터용 그룹
UENUM(BlueprintType)
enum class EItemGroup : uint8
{
	None,
	Weapon,
	Armor,
	Helmet,
	Growth,
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
enum class ETargetType : uint8
{
	InRange,
	LowHpInRange,
	HighHpInRange,
	Camp,
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
