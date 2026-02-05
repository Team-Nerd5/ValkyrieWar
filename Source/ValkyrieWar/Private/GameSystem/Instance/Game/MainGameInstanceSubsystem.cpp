// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/MainGameInstanceSubsystem.h"
#include "GameSystem/Instance/Game/MainSaveGame.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "Kismet/GameplayStatics.h"

void UMainGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CachedSaveGame = Cast<UMainSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass()));
	LoadGame();
}

void UMainGameInstanceSubsystem::Deinitialize()
{
	SaveGame();
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

			UE_LOG(LogTemp, Log, TEXT("저장파일 로딩 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("저장파일 로드 실패"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("저장된 파일이 없다."));
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

	// 게임매니저 가져오기 시도
	if (UGameManager* GameManager = Cast<UGameManager>(GetGameInstance()))
	{
		// 열고싶은 맵 찾기
		TSoftObjectPtr<UWorld> TargetMap = GameManager->GetMapObject(MapType);

		// 찾았을 때만
		if (TargetMap)
		{
			CurrentMapType = MapType;

			UWorld* CurrentWorld = GetWorld();
			if (CurrentWorld)
			{
				FLatentActionInfo LatentInfo;
				LatentInfo.CallbackTarget = this;
				LatentInfo.ExecutionFunction = FName("OnLevelLoadComplete");
				LatentInfo.Linkage = 0;
				LatentInfo.UUID = FMath::Rand();

				

			}

		}
		
	}

}
