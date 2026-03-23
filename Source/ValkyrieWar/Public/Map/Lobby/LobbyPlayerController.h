// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/Enum/StateEnums.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	ALobbyPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void ChageGameState(ELobbyState InState);

	void SetActorCamera(FName InLevelName);

	UFUNCTION()
	void LoadLobbyLevel();

	UFUNCTION()
	void LoadGachaLevel(int32 InAmount);

	UFUNCTION()
	void OnLobbyLevelLoaded();

	UFUNCTION()
	void OnGachaLevelLoaded();

private:
	int32 GachaAmount = 0;
};
