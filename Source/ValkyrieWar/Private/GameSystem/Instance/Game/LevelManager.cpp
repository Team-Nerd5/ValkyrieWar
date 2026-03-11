// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/LevelManager.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Library/Data/GameDataFactory.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Object/SaveGame/AccountSaveGame.h"

#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Loading/LoadingWidget.h"
#include "Widget/HUD/LoginWidget.h"

void ULevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);	
}

void ULevelManager::Deinitialize()
{

    Super::Deinitialize();
}

void ULevelManager::LoadMap(EMapType InMapType, bool bShowLoading, bool bIsLoadData)
{
	if (UGameManager* GameManager = Cast<UGameManager>(GetGameInstance()))
	{
		TSoftObjectPtr<UWorld> MapObject = GameManager->GetMapObject(InMapType);
		if (MapObject.IsValid())
		{
			LoadLevelAsync(MapObject, bShowLoading);

			if (bIsLoadData)
			{
				StartDataLoading();
			}
			else
			{
				DataLoadProgress = 1.0f;
				StartMapLoading();
			}
		}

	}

}

void ULevelManager::LoadLevelAsync(TSoftObjectPtr<UWorld> InMap, bool bShowLoading)
{
	if (InMap.IsNull()) return;

	TargetMap = InMap;
	DataLoadProgress = 0.0f;

	if (bShowLoading)
	{
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UIManager->OpenUI<ULoadingWidget>(EUIType::Loading);
		}
	}
}

void ULevelManager::InitEvent()
{
	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	if (EventSystem)
	{
		EventSystem->Login.OnDataLoadComplete.AddDynamic(this, &ULevelManager::OnDataLoadComplete);
	}
}

void ULevelManager::StartDataLoading()
{
	CurrentState = ELoadingState::LoadingData;

	//데이터 로드..
	UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();
	if (DataManager)
	{
		DataLoadTask += DataManager->CreateData();
	}
	USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>();
	if (SaveManager)
	{
		DataLoadTask += SaveManager->LoadAllData();
	}

	DataLoadProgress = DataLoadedTask / DataLoadTask;
}

void ULevelManager::OnDataLoadCompleted()
{
	//신규 계정일 때만..
	if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
	{
		if (SaveManager->IsNewAccount())
		{
			UGameDataFactory::GenerateValkyrie(110001, GetGameInstance());
		}
	}

	StartMapLoading();
}

void ULevelManager::StartMapLoading()
{
	CurrentState = ELoadingState::LoadingMap;

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	MapLoadHandle = StreamableManager.RequestAsyncLoad(
		TargetMap.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &ULevelManager::OnMapLoadCompleted)
	);
}

void ULevelManager::OnMapLoadCompleted()
{
	CurrentState = ELoadingState::None;

	if (MapLoadHandle.IsValid())
	{
		MapLoadHandle->ReleaseHandle();
		MapLoadHandle.Reset();
	}

	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		//Persistant는 명시적 제거
		UIManager->CloseUI<ULoginWidget>(EUIType::Login);
		UIManager->CloseAllPopupUI();
	}

	// 최종 맵 이동
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, TargetMap);
}

float ULevelManager::CalculateCombinedProgress() const
{
	const float DataWeight = 0.3f; // 데이터 로드 비중 30%
	const float MapWeight = 0.7f;  // 맵 로드 비중 70%

	if (CurrentState == ELoadingState::LoadingData)
	{
		return DataLoadProgress * DataWeight;
	}
	else if (CurrentState == ELoadingState::LoadingMap)
	{
		float MapProgress = MapLoadHandle.IsValid() ? MapLoadHandle->GetProgress() : 0.0f;
		return DataWeight + (MapProgress * MapWeight);
	}

	return 0.0f;
}

void ULevelManager::OnDataLoadComplete()
{
	DataLoadedTask += 1.0f;

	if (static_cast<int32>(DataLoadedTask) == DataLoadTask)
	{
		OnDataLoadCompleted();
	}
}
