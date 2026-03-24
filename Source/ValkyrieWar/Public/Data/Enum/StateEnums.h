// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ELoginState : uint8
{
	Init,
	CheckAccount,
	LoadAccount,
	ReadyToStart,
	CheckNickname,
	StartCreateAccount,
	CreateAccount,
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
	MoveToNextLevel,// 다음레벨로 이동
};

UENUM(BlueprintType)
enum class ELobbyState : uint8
{
	Init,			//로비 데이터 호출 시작
	Ready,			//인벤토리 등 데이터 로드 완료, UI 호출
	Gacha,
	MoveToStage,	//스테이지로 레벨 전환
};

UENUM(BlueprintType)
enum class ELoadingState : uint8
{
	None,
	LoadingData, // 데이터 로드 중
	LoadingMap   // 맵 애셋 로드 중
};
