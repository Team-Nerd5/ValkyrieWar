// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/LoginGameState.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/LevelManager.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "Widget/HUD/LoginWidget.h"
#include "Widget/Popup/Login/CreateAccountWidget.h"

#include <Kismet/GameplayStatics.h>

void ALoginGameState::ChangeGameState(ELoginState InState)
{
	State = InState;

	switch (State)
	{
	case ELoginState::Init:
		//로그인 UI를 오픈
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->OpenUI<ULoginWidget>(EUIType::Login);

			ChangeGameState(ELoginState::CheckAccount);
		}
		break;	
	case ELoginState::CheckAccount:
		if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
		{
			bIsAccountExist = SaveManager->IsAcountExist();
			ChangeGameState(ELoginState::LoadAccount);
		}
		break;
	case ELoginState::LoadAccount:
		if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
		{
			SaveManager->LoadData(ESaveType::Account);
			ChangeGameState(ELoginState::ReadyToStart);
		}
		break;

	case ELoginState::ReadyToStart:

		if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			EventSystem->Login.OnReadyToStart.Broadcast();
		}
		break;

	case ELoginState::CheckNickname:
		if (bIsAccountExist)
		{
			if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
			{
				SaveManager->GetUserId();
				ChangeGameState(ELoginState::MoveToLobby);
			}
		}
		else
		{
			ChangeGameState(ELoginState::StartCreateAccount);
		}
		break;
	case ELoginState::StartCreateAccount:
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->OpenUI<UCreateAccountWidget>(EUIType::PopupCreateAccount);
		}
		break;
	case ELoginState::CreateAccount:
		ChangeGameState(ELoginState::MoveToLobby);
		break;
	case ELoginState::MoveToLobby:
		if (UGameManager* GameManager = GetGameInstance<UGameManager>())
		{
			if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
			{
				UIManager->CloseAllPopupUI();
				UIManager->ResetAllUIStates();
			}

			if (ULevelManager* LevelManager = GetGameInstance()->GetSubsystem<ULevelManager>())
			{
				LevelManager->LoadLobbyMap();
			}
		}

		//레벨 전환
		break;
	}
}
