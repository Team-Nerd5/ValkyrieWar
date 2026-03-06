// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/LobbyGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/LevelManager.h"

#include "Widget/HUD/LobbyWidget.h"
#include "Widget/HUD/TopMenuWidget.h"
#include "Widget/Loading/LoadingWidget.h"

void ALobbyGameState::ChangeState(ELobbyState InState)
{
	State = InState;

	switch (State)
	{
	case ELobbyState::Init:
		//기본 UI 표기
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->OpenUI<ULobbyWidget>(EUIType::Lobby);
			UIManager->CloseUI<ULoadingWidget>(EUIType::Loading);
		}
		ChangeState(ELobbyState::Ready);
		//캐릭터 생성/배치
		break;
	case ELobbyState::Ready:
		//그냥 로비 표기상태..
		break;
	case ELobbyState::MoveToStage:
		//UI 지워주고 스테이지로 레벨전환
		if (ULevelManager* LevelManager = GetGameInstance()->GetSubsystem<ULevelManager>())
		{
			LevelManager->LoadMap(EMapType::Battle, true);
		}		
		break;
	}
}
