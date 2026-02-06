// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Level/LevelManager.h"

void ULevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ULevelManager::BeginLoadingScreen);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULevelManager::EndLoadingScreen);
}

void ULevelManager::Deinitialize()
{
    FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

    Super::Deinitialize();
}

void ULevelManager::BeginLoadingScreen(const FString& MapName)
{
    UE_LOG(LogTemp, Warning, TEXT("로딩창 열림"));
}

void ULevelManager::EndLoadingScreen(UWorld* InLoadedWorld)
{
    UE_LOG(LogTemp, Warning, TEXT("로딩창 닫힘"));
}
