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

UENUM(BlueprintType)
enum class EUnitCharacterType : uint8
{
	SwordMan,
	SpearMan,
	Archer,
	General,
	Wizard,
};

UENUM(BlueprintType)
enum class EGameplayCueOrder : uint8
{
	OnExecute,		//실행 시점에 동작(사전 이펙트 등)
	OnNotify,		//타격 시점
	InProjectile,	//발사체에 실어서 발사체 피격 액터 위치에 전달
};
