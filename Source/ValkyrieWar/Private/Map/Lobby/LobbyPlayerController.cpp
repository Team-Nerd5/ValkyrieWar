// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Lobby/LobbyPlayerController.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/State/Game/LobbyGameState.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Object/Cheat/LobbyCheatManager.h"

#include "Camera/CameraActor.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	CheatClass = ULobbyCheatManager::StaticClass();
}

void ALobbyPlayerController::BeginPlay()
{
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Lobby.OnLobbyStateChanged.AddDynamic(this, &ALobbyPlayerController::ChageGameState);
	}

	ACameraActor* FoundCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));

	if (FoundCamera)
	{
		SetViewTargetWithBlend(FoundCamera, 0.0f);
	}

	bShowMouseCursor = true;

	ChageGameState(ELobbyState::Init);
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Lobby.OnLobbyStateChanged.RemoveDynamic(this, &ALobbyPlayerController::ChageGameState);
	}

}

void ALobbyPlayerController::ChageGameState(ELobbyState InState)
{
	if (ALobbyGameState* State = GetWorld()->GetGameState<ALobbyGameState>())
	{
		State->ChangeState(InState);
	}
}
