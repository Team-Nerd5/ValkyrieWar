// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Login/Controller/LoginPlayerController.h"
#include "GameSystem/State/Game/LoginGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void ALoginPlayerController::BeginPlay()
{
	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);

	if (EventSystem)
	{
		EventSystem->Login.OnLoginStateChanged.AddDynamic(this, &ALoginPlayerController::ChageGameState);
	}

	ChageGameState(ELoginState::Init);
}

/// <summary>
/// UI에서 버튼 누르고 동작시킬 때 이벤트 호출시킴
/// </summary>
/// <param name="InState"></param>
void ALoginPlayerController::ChageGameState(ELoginState InState)
{
	if (ALoginGameState* State = GetWorld()->GetGameState<ALoginGameState>())
	{
		State->ChangeGameState(InState);
	}
}
