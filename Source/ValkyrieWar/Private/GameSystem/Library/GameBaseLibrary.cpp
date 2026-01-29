// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/GameBaseLibrary.h"

UWorldEventSystem* UGameBaseLibrary::GetWorldEventSystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
		return nullptr;

	return World->GetSubsystem<UWorldEventSystem>();
}
