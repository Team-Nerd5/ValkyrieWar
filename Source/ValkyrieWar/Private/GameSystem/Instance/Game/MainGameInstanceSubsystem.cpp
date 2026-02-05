// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/MainGameInstanceSubsystem.h"
#include "GameSystem/Instance/Game/MainSaveGame.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "UObject/Package.h"

void UMainGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CachedSaveGame = Cast<UMainSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass()));
	LoadGame();
}

void UMainGameInstanceSubsystem::Deinitialize()
{
	if (bIsLoggedIn)
	{
		SaveGame();
	}
	CachedSaveGame = nullptr;

	Super::Deinitialize();
}

void UMainGameInstanceSubsystem::SetPlayerData(const FPlayerSaveData& InPlayerData)
{
	CurrentPlayerData = InPlayerData;

	SaveGame();
}

void UMainGameInstanceSubsystem::SaveGame()
{
	if (CachedSaveGame)
	{
		CachedSaveGame->PlayerData = CurrentPlayerData;

		bool bSaveSuccess = UGameplayStatics::SaveGameToSlot(CachedSaveGame, SaveSlotName, SaveIndex);
		if (bSaveSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("저장 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("저장 실패"));
		}

	}

}

void UMainGameInstanceSubsystem::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveIndex))
	{
		UMainSaveGame* LoadedSaveGame = Cast<UMainSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveIndex));

		if (LoadedSaveGame)
		{
			CurrentPlayerData = LoadedSaveGame->PlayerData;
			CachedSaveGame->PlayerData = CurrentPlayerData;

			if (CurrentPlayerData.bHasEverLoggedIn)
			{
				bIsLoggedIn = true;
				UE_LOG(LogTemp, Log, TEXT("로그인 기록 있음. 로비로 이동"));
				TransitLevel(EMapType::Lobby);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("저장파일은 있는데 로그인 기록 없음. 로그인으로 이동"));
				TransitLevel(EMapType::Login);
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("저장파일 로드 실패"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("저장파일 없음. 로그인으로 이동"));
		TransitLevel(EMapType::Login);
	}

}

void UMainGameInstanceSubsystem::AddGold(int32 InGold)
{
	CurrentPlayerData.Gold += InGold;

	SaveGame();
}

void UMainGameInstanceSubsystem::AddTicket(int32 InTicket)
{
	CurrentPlayerData.Ticket += InTicket;

	SaveGame();
}

void UMainGameInstanceSubsystem::TransitLevel(EMapType MapType)
{
	OnLevelTransitStarted.Broadcast(MapType);

	// 일단 저장
	SaveGame();

	// UI매니저 가져오기 시도
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		// 모든 UI상태 초기화
		UIManager->ResetAllUIStates();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UI매니저 불러오기 실패"));
	}

	// 게임매니저 가져오기 시도
	if (UGameManager* GameManager = Cast<UGameManager>(GetGameInstance()))
	{
		// 열고싶은 맵 찾기
		TSoftObjectPtr<UWorld> TargetMap = GameManager->GetMapObject(MapType);

		// 찾았을 때만
		if (TargetMap)
		{
			CurrentMapType = MapType;

			UGameplayStatics::OpenLevel(this, TargetMap.GetLongPackageFName());

			// 타이머로 로딩 완료 체크
			//GetWorld()->GetTimerManager().SetTimer(
			//	LevelTransitCheckTimer,
			//	this,
			//	&UMainGameInstanceSubsystem::LevelTransitComplete,
			//	0.1f,
			//	true
			//);

		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("TargetMap 불러오기 실패"));
		}

	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("게임매니저 불러오기 실패"));
	}

}

void UMainGameInstanceSubsystem::LevelTransitComplete()
{
	//if (레벨 로드 완료 조건)
	//{
	//	GetWorld()->GetTimerManager().ClearTimer(LevelTransitCheckTimer);
	//	OnLevelTransitCompleted.Broadcast(CurrentMapType);
	//	// 로딩화면 숨기기
	//}

}

void UMainGameInstanceSubsystem::LoginPlayer(const FString& InPlayerName)
{
	bIsLoggedIn = true;

	CurrentPlayerData.PlayerName = InPlayerName;
	CurrentPlayerData.bHasEverLoggedIn = true;
	SaveGame();

	TransitLevel(EMapType::Lobby);

}
