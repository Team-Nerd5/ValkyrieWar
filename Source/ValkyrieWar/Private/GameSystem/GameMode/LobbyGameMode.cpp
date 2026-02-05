// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/GameMode/LobbyGameMode.h"
#include "GameSystem/State/Game/LobbyGameState.h"

void ALobbyGameMode::BeginPlay()
{
	if (ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>())
	{
		LobbyState->ChangeState(ELobbyState::Init);
	}
}
