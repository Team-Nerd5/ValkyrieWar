// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/LevelManager.h"
#include "LoadingScreenModule.h"

void ULevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // BeginLoadingScreen 함수를 PostLoadMap 델리게이트에 바인딩
    // 이렇게 하면 맵이 로드되기 전에 우리 함수가 호출되고 로딩 화면을 시작함
    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ULevelManager::BeginLoadingScreen);

    // EndLoadingScreen 함수를 PostLoadMapWithWorld 델리게이트에 바인딩
    // 이렇게 하면 맵이 로드된 후에 우리 함수가 호출됨
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULevelManager::EndLoadingScreen);
}

void ULevelManager::Deinitialize()
{
    // 바인딩 해제
    FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

    Super::Deinitialize();
}

void ULevelManager::BeginLoadingScreen(const FString& MapName)
{
    UE_LOG(LogTemp, Warning, TEXT("로딩창 열림"));


    // 로딩 화면 모듈을 가져오기 시도
    FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule");
    if (LoadingScreenModule != nullptr)
    {
        // 모듈을 찾음 - 로딩 화면 시작
        LoadingScreenModule->StartLoadingScreen(MapName);
    }
    else
    {
        // 모듈을 찾지 못함 - 경고 로그 출력
        UE_LOG(LogTemp, Warning, TEXT("MyGameInstance::BeginLoadingScreen: LoadingScreenModule not found"));
    }
}

void ULevelManager::EndLoadingScreen(UWorld* InLoadedWorld)
{
    UE_LOG(LogTemp, Warning, TEXT("로딩창 닫힘"));
}
