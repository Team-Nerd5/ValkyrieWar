// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Lobby/LobbyPlayerController.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/State/Game/LobbyGameState.h"

#include "GameSystem/Library/GameBaseLibrary.h"

void ALobbyPlayerController::BeginPlay()
{
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Lobby.OnLobbyStateChanged.AddDynamic(this, &ALobbyPlayerController::ChageGameState);
	}

	bShowMouseCursor = true;

	ChageGameState(ELobbyState::Init);
}

void ALobbyPlayerController::ChageGameState(ELobbyState InState)
{
	if (ALobbyGameState* State = GetWorld()->GetGameState<ALobbyGameState>())
	{
		State->ChangeState(InState);
	}
}
