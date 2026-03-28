// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/LobbyGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/LevelManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Widget/HUD/LobbyWidget.h"
#include "Widget/HUD/TopMenuWidget.h"
#include "Widget/Loading/LoadingWidget.h"
#include "Kismet/GameplayStatics.h"

void ALobbyGameState::ChangeState(ELobbyState InState)
{
	State = InState;

	switch (State)
	{
	case ELobbyState::Init:

		if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			EventSystem->Lobby.OnLoadLobby.Broadcast();
		}
		//기본 UI 표기
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->OpenUI<ULobbyWidget>(EUIType::Lobby);
		}
		//컨트롤러..
		
		ChangeState(ELobbyState::Ready);
		//캐릭터 생성/배치
		break;
	case ELobbyState::Ready:
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->CloseUI<ULoadingWidget>(EUIType::Loading);
		}
		break;
	case ELobbyState::Gacha:

		break;
	case ELobbyState::MoveToStage:
		//UI 지워주고 스테이지로 레벨전환
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->CloseUI<ULobbyWidget>(EUIType::Lobby);
		}
		if (ULevelManager* LevelManager = GetGameInstance()->GetSubsystem<ULevelManager>())
		{
			LevelManager->LoadMap(EMapType::Battle, true);
		}		
		break;
	}
}
