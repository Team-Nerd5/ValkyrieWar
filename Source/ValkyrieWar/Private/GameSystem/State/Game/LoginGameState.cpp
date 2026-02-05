// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/LoginGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"

void ALoginGameState::ChangeGameState(ELoginState InState)
{
	State = InState;

	switch (State)
	{
	case ELoginState::Init:
		//로그인 UI를 오픈
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			//UIManager->OpenUI(EUIType::Login);
			//오픈되면 로딩 이런느낌좀 내봐..?
		}
		break;
	case ELoginState::LoadData:
		//화면 터치하면 계정 데이터 로드 시도
		//파일이 없으면 CreateAccount로
		//있으면 CheckAccount
		break;
	case ELoginState::CheckAccount:
		//데이터 세팅 후 MoveToLobby로
		break;

	case ELoginState::CreateAccount:
		//계정 생성 UI 띄워줌
		//계정생성 취소 시 경고팝업 -> 확인 시 종료
		//계정 생성 성공 시 MakeSaveData로
		break;
	case ELoginState::MakeSaveData:
		//데이터 저장 후 CheckAccount로
		break;

	case ELoginState::MoveToLobby:
		//로비로 레벨 전환

		//켜진 UI전부 종료
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->CloseAllPopupUI();
			UIManager->ResetAllUIStates();
		}

		//레벨 전환
		break;
	}
}
