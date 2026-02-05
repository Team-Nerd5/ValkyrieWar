// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/LobbyGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"

void ALobbyGameState::ChangeState(ELobbyState InState)
{
	State = InState;

	switch (State)
	{
	case ELobbyState::Init:
		//데이터 로드
		break;
	case ELobbyState::Ready:
		//UIManager에서 로비 UI 호출
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			
		}
		break;
	case ELobbyState::MoveToStage:
		//UI 지워주고 스테이지로 레벨전환
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->CloseAllPopupUI();
			UIManager->ResetAllUIStates();
		}
		break;
	}
}
