// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Save/SaveManager.h"

#include "Object/Save/ValkyrieWarSaveGame.h"

#include "Kismet/GameplayStatics.h"

void USaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CachedSaveGame = Cast<UValkyrieWarSaveGame>(UGameplayStatics::CreateSaveGameObject(UValkyrieWarSaveGame::StaticClass()));
	LoadGame();
}

void USaveManager::Deinitialize()
{
	SaveGame();
	CachedSaveGame = nullptr;

	Super::Deinitialize();
}

void USaveManager::SetPlayerAccountData(const FPlayerAccountData& InPlayerAccountData)
{
	CurrentPlayerAccountData = InPlayerAccountData;
	
	SaveGame();
}

void USaveManager::SaveGame()
{
	if (CachedSaveGame)
	{
		CachedSaveGame->PlayerAccountData = CurrentPlayerAccountData;

		bool bIsSuccess = UGameplayStatics::SaveGameToSlot(CachedSaveGame, SaveSlotName, SaveIndex);
		if (bIsSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("저장 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("저장 실패"));
		}
	}

}

void USaveManager::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveIndex))
	{
		UValkyrieWarSaveGame* LoadedSaveGame = Cast<UValkyrieWarSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveIndex));

		if (LoadedSaveGame)
		{
			CurrentPlayerAccountData = LoadedSaveGame->PlayerAccountData;
			CachedSaveGame->PlayerAccountData = CurrentPlayerAccountData;
			UE_LOG(LogTemp, Log, TEXT("저장파일 로드 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("저장파일 로드 실패"));
		}
	}

}
