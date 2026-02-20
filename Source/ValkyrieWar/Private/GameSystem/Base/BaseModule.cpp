// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseModule.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void UBaseModule::Initialize(UGameManager* InGameManager)
{
	GameManager = InGameManager;
}

void UBaseModule::SendDataLoadComplete()
{
	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	if (EventSystem)
	{
		EventSystem->Login.OnDataLoadComplete.Broadcast();
	}
}
