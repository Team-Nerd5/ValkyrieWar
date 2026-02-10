// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EUIType : uint8
{
	//HUD
	Login,
	Lobby,
	Battle,

	//로딩은 로딩 레벨 자체에 열어두는것도

	//Lobby Popup
	PopupCharacterInfo,

	PopupInventory,

	PopupGacha,
	PopupGachaResult,
	PopupGachaRate,

	PopupUnitEnforce,
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
enum class ELoginState : uint8
{
	Init,
	LoadData,
	CheckAccount,
	CreateAccount,
	MakeSaveData,
	MoveToLobby,
};


UENUM(BlueprintType)
enum class EBattleState : uint8
{
	Init,		//최초 상태. 데이터 로드 시작
	Ready,		//데이터 및 필요 오브젝트 로드 완료
	Start,		//게임 시작 연출 발키리 뭐 동작 정도?
	Play,		//전투 중
	Win,		//전투 승리로 종료
	Defeat,		//전투 패배로 종료
	TimeOver,	//시간초과로 종료(패배)
	MoveToLobby,//로비로 이동
};

UENUM(BlueprintType)
enum class ELobbyState : uint8
{
	Init,			//로비 데이터 호출 시작
	Ready,			//인벤토리 등 데이터 로드 완료, UI 호출
	MoveToStage,	//스테이지로 레벨 전환
};

UENUM(BlueprintType)
enum class EPoolTypes : uint8
{
	// 디폴트(무효)
	None,

	// 캐릭터
	BaseCharacter,

	// 폰
	BasePawn,

	// 액터
	BaseActor,

	// 아군 근딜
	Melee_Friend,

	// 적 근딜
	Melee_Enemy,
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
enum class ETeam : uint8
{
	TeamA,
	TeamB
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
enum class EAttributeType : uint8
{
	Health,
	Defence,
	Attack,
	MoveSpeed,
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

UENUM(BlueprintType)
enum class EWeaponAnimType : uint8
{
	None UMETA(DisplayName = "None"), //맨손

	Test_Melee UMETA(DisplayName = "Melee"), // 뭔가 근접무기
	Test_Range UMETA(DisplayName = "Range"), // 뭔가 원거리 무기
	Test_Magic UMETA(DisplayName = "Magic"), // 뭔가 마법무기
};

UENUM(BlueprintType)
enum class ETableDataType : uint8
{
	Character,
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

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UAnimMontage> AttackMontage;
};
