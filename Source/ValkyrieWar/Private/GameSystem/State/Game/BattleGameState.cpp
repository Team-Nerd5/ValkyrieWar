// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/BattleGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"

void ABattleGameState::ChangeState(EBattleState InState)
{
	State = InState;

	switch (State)
	{
	case EBattleState::Init:
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			//전투 UI 오픈
		}
		break;

	case EBattleState::Ready:
		break;

	case EBattleState::Start:
		break;

	case EBattleState::Play:
		break;

	case EBattleState::Win:
		//결과 UI를 Win으로 열어줌
		break;
	case EBattleState::Defeat:
		//결과 UI를 Defeat으로 열여줌
		break;
	case EBattleState::TimeOver:
		//결과 UI를 Draw로 열어줌
		break;
	case EBattleState::MoveToLobby:

		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->CloseAllPopupUI();
			UIManager->ResetAllUIStates();
		}
		break;
	}
}
