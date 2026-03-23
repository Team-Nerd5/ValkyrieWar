// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ValkyrieWarGameMode.generated.h"

class AValkyrieCharacter;

UCLASS(minimalapi)
class AValkyrieWarGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AValkyrieWarGameMode();
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	AValkyrieCharacter* SpawnValkyrie(AController* InController, TSubclassOf<APawn> PawnClassToSpawn);
};



