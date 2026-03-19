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
		EventSystem->Lobby.OnLoadLobby.AddDynamic(this, &ALobbyPlayerController::LoadLobbyLevel);
		EventSystem->Lobby.OnLoadGacha.AddDynamic(this, &ALobbyPlayerController::LoadGachaLevel);

	}	
	ChageGameState(ELobbyState::Init);
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Lobby.OnLobbyStateChanged.RemoveDynamic(this, &ALobbyPlayerController::ChageGameState);
		EventSystem->Lobby.OnLoadLobby.RemoveDynamic(this, &ALobbyPlayerController::LoadLobbyLevel);
		EventSystem->Lobby.OnLoadGacha.RemoveDynamic(this, &ALobbyPlayerController::LoadGachaLevel);
	}

}

void ALobbyPlayerController::ChageGameState(ELobbyState InState)
{
	if (ALobbyGameState* State = GetWorld()->GetGameState<ALobbyGameState>())
	{
		State->ChangeState(InState);
	}
}

void ALobbyPlayerController::SetActorCamera(FName InLevelName)
{
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), InLevelName);

	if (StreamingLevel && StreamingLevel->IsLevelLoaded())
	{
		// 2. 스트리밍 객체에서 실제 로드된 ULevel 메모리를 가져옵니다.
		ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel();

		if (LoadedLevel)
		{
			for (AActor* Actor : LoadedLevel->Actors)
			{
				// 1. 배열의 해당 인덱스가 비어있지 않은지 반드시 먼저 확인합니다.
				if (Actor != nullptr)
				{
					// 2. 그 다음 클래스를 확인하고 캐스팅합니다.
					if (Actor->IsA(ACameraActor::StaticClass()))
					{
						ACameraActor* MyCam = Cast<ACameraActor>(Actor);
						// MyCam 사용

						SetViewTargetWithBlend(MyCam, 0.0f);
						break;
					}
				}
			}
		}
	}

	bShowMouseCursor = true;
}

void ALobbyPlayerController::LoadLobbyLevel()
{
	

	// 스트리밍 레벨 객체를 먼저 가져옵니다.
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("Lobby"));

	if (StreamingLevel)
	{
		// 레벨 로드가 완료되었을 때 실행될 함수를 델리게이트에 연결합니다.
		StreamingLevel->OnLevelLoaded.AddDynamic(this, &ALobbyPlayerController::OnLobbyLevelLoaded);

		// 비동기 로드 시작
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Lobby"), true, false, FLatentActionInfo());
	}
}

void ALobbyPlayerController::LoadGachaLevel()
{
	

	// 스트리밍 레벨 객체를 먼저 가져옵니다.
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("GachaMap"));

	if (StreamingLevel)
	{
		// 레벨 로드가 완료되었을 때 실행될 함수를 델리게이트에 연결합니다.
		StreamingLevel->OnLevelLoaded.AddDynamic(this, &ALobbyPlayerController::OnGachaLevelLoaded);

		// 비동기 로드 시작
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName("GachaMap"), true, false, FLatentActionInfo());
	}
}

void ALobbyPlayerController::OnLobbyLevelLoaded()
{
	if (ULevelStreaming* GachaLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("GachaMap")))
	{
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("GachaMap"), FLatentActionInfo(), false);
	}
	SetActorCamera(FName("Lobby"));
}

void ALobbyPlayerController::OnGachaLevelLoaded()
{
	if (ULevelStreaming* LobbyLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("Lobby")))
	{
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("Lobby"), FLatentActionInfo(), false);
	}
	SetActorCamera(FName("GachaMap"));
}
