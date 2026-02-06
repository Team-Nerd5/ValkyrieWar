// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULevelManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	void BeginLoadingScreen(const FString& MapName);

	void EndLoadingScreen(UWorld* InLoadedWorld);

};
