// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/MainGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystem/Instance/Game/MainSaveGame.h"

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

void UMainGameInstanceSubsystem::SaveGame()
{
	if (CachedSaveGame)
	{
		CachedSaveGame->SavedGold = Gold;
		CachedSaveGame->SavedTicket = Ticket;

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
			Gold = LoadedSaveGame->SavedGold;
			Ticket = LoadedSaveGame->SavedTicket;

			CachedSaveGame->SavedGold = Gold;
			CachedSaveGame->SavedTicket = Ticket;
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
	Gold += InGold;

	SaveGame();
}

void UMainGameInstanceSubsystem::AddTicket(int32 InTicket)
{
	Ticket += InTicket;

	SaveGame();
}
