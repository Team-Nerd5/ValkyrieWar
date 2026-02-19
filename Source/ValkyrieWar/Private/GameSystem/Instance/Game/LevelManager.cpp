// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/LevelManager.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Loading/LoadingWidget.h"

void ULevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

}

void ULevelManager::Deinitialize()
{

    Super::Deinitialize();
}

void ULevelManager::LoadLevelAsync(TSoftObjectPtr<UWorld> InMap)
{
	if (InMap.IsNull()) return;

	TargetMap = InMap;
	DataLoadProgress = 0.0f;

	//UIManager 로딩 위젯
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->OpenUI<ULoadingWidget>(EUIType::Loading);
	}

	// 3. 데이터 로딩부터 시작
	StartDataLoading();
}

void ULevelManager::StartDataLoading()
{
	CurrentState = ELoadingState::LoadingData;

	DataLoadProgress = 1.0f;
	OnDataLoadCompleted();
}

void ULevelManager::OnDataLoadCompleted()
{
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
		//클래스 만들고..
		UIManager->CloseUI<ULoadingWidget>(EUIType::Loading);
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
