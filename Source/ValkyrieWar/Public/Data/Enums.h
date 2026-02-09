// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
	// 캐릭터
	BaseCharacter,

	// 폰
	BasePawn,

	// 액터
	BaseActor,
};

UENUM(BlueprintType)
enum class EInputControlMode : uint8
{
	Manual UMETA(DisplayName = "Manual Mode"),
	Auto UMETA(DisplayName = "Auto Mode")
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
	Goods,
	Equip,
	Item,
};

//아이템 필터용 그룹
UENUM(BlueprintType)
enum class EItemGroup : uint8
{
	Weapon,
	Armor,
	Helmet,
	Growth,
};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
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
