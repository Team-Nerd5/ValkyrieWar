// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/Enum/StateEnums.h"
#include "LobbyGameMode.generated.h"

class UValkyrieData;
class AValkyrieCharacter;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	AValkyrieCharacter* SpawnValkyire(UValkyrieData* InData, AController* InController, FName InTag);

private:
	FName PlayerStartTag;
};
