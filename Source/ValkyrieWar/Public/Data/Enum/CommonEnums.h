// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EUIType : uint8
{
	None,
	//HUD
	Login,
	Lobby,
	Battle,

	//로딩은 로딩 레벨 자체에 열어두는것도
	Loading,
	PopupCreateAccount,

	//Lobby Popup
	PopupCharacterInfo,

	PopupInventory,

	PopupGacha,
	PopupGachaResult,
	PopupGachaRate,

	PopupUnitUpgrade,

	PopupStageList,
	PopupStageDetail,

	// Battle Popup
	PopupBattleResult,
	PopupBattlePause,
};

UENUM(BlueprintType)
enum class EMapType : uint8
{
	Login,
	Lobby,
	Battle,
	Loading,
};

UENUM(BlueprintType)
enum class EPoolTypes : uint8
{
	// 디폴트(무효)
	None,

	// 아군 유닛 풀타입
	Ally_Archer,
	Ally_Sword,
	Ally_General,
	Ally_Spear,
	Ally_Wizard,

	Ally_Archer_Uncommon,
	Ally_Sword_Uncommon,
	Ally_General_Uncommon,
	Ally_Spear_Uncommon,
	Ally_Wizard_Uncommon,

	// 적 유닛 풀타입
	Enemy_Sword,
	Enemy_Knight,
	Enemy_Defender,
	Enemy_Hammer,
	Enemy_Archer,
	Enemy_Mage,

	//Projectile
	EnemyUnit_BowAttack,
	EnemyUnit_StaffAttack,
	AllyUnit_BowAttack,
	AllyUnit_StaffAttack,
	Valkyrie_BowAttack,
	Valkyrie_StaffAttack_1,
	Valkyrie_StaffAttack_2,
	Valkyrie_BowSkill_1,
	Valkyrie_StaffSkill_1,
};

UENUM(BlueprintType)
enum class EInputControlMode : uint8
{
	Manual UMETA(DisplayName = "Manual Mode"),
	Auto UMETA(DisplayName = "Auto Mode")
};

UENUM(BlueprintType)
enum class EUnitCombatType : uint8
{
	Melee,
	Ranged
};

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	None,
	Ally,
	Enemy
};

// 타깃 선택 룰셋(서브시스템은 attacker 제한/슬롯 정책을 유지하고,
// 여기서는 "어떤 후보를 우선할지"만 결정한다)
UENUM(BlueprintType)
enum class ETargetingPolicy : uint8
{
	Auto,                       // 유닛 타입(근/원거리) 기반 자동 선택
	NearWallEnemyThenBase,      // 내 성벽(또는 WallAnchor) 근처의 적을 우선, 없으면 적 기지
	NearestEnemyThenBase,       // 가장 가까운 적, 없으면 적 기지
	FarthestInAttackRangeThenBase // 공격 사거리(AttackRange) 안에서 가장 먼 적, 없으면 적 기지
};

UENUM(BlueprintType)
enum class EReservationState : uint8
{
	None,
	Reserved,        // 슬롯 확보 완료
	MovingToTarget,  // 예약 타깃으로 이동 중
	InCombat         // 공격 중
};

UENUM(BlueprintType)
enum class EInventoryFilterType : uint8
{
	All,
	Equipment,
	Item,
};

UENUM(BlueprintType)
enum class ECharacterInfoFilterType : uint8
{
	All,
	Weapon,
	Armor,
	Helmet,
};

UENUM(BlueprintType)
enum class ETabType : uint8
{
	Inventory,
	CharacterInfo,
};
