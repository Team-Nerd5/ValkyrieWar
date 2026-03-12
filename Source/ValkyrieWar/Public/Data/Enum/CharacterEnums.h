// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class EWeaponAnimType : uint8
{
	None UMETA(DisplayName = "None"), //맨손

	Test_Melee UMETA(DisplayName = "Melee"), // 뭔가 근접무기
	Test_Range UMETA(DisplayName = "Range"), // 뭔가 원거리 무기
	Test_Magic UMETA(DisplayName = "Magic"), // 뭔가 마법무기
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Melee,
	Projectile,
	OnTarget,		//타겟에 즉발성(번개 마법 등)
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Bow,
	Staff,
	SwordAndShield,
	Spear,

};

UENUM(BlueprintType)
enum class EStatusType : uint8
{
	Attack,
	Health,
	Defence,
};

UENUM(BlueprintType)
enum class EValkyrieModeType : uint8
{
	None,
	Auto,
	Manual,
};

UENUM(BlueprintType)
enum class EProjectileShootType : uint8
{
	None, // 일직선 발사
	Multiple,	//기본 발사체 기준으로 개수만큼 추가
};

