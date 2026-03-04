// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/BattleGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"

#include "Widget/HUD/BattleWidget.h"
#include "Widget/Loading/LoadingWidget.h"

void ABattleGameState::ChangeState(EBattleState InState)
{
	State = InState;

	switch (State)
	{
	case EBattleState::Init:
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UBattleWidget* BattleUI = UIManager->OpenUI<UBattleWidget>(EUIType::Battle);

			if (!BattleUI)
			{
				UE_LOG(LogTemp, Error, TEXT("BattleUI is not exist!! Can't start game"));
				return;
			}

			UIManager->CloseUI<ULoadingWidget>(EUIType::Loading);

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				AValkyrieCharacterController* VPC = Cast<AValkyrieCharacterController>(PC);
				if (VPC)
				{
					VPC->SetBattleUI(BattleUI);
				}
			}
		}
		break;

	case EBattleState::Ready:
		break;

	case EBattleState::Start:
		break;

	case EBattleState::Play:
		break;

	case EBattleState::Win:
		UE_LOG(LogTemp, Warning, TEXT("Win!"));
		//결과 UI를 Win으로 열어줌
		break;
	case EBattleState::Defeat:
		UE_LOG(LogTemp, Warning, TEXT("Lose.."));
		//결과 UI를 Defeat으로 열여줌
		break;
	case EBattleState::TimeOver:
		//결과 UI를 Draw로 열어줌
		break;
	case EBattleState::MoveToLobby:

		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			//레벨 전환으로 가야할 듯..
		}
		break;
	}
}
