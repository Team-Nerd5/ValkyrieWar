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
	//발사체인지?
	InRange,
	Projectile,
	TargetRange,
};
