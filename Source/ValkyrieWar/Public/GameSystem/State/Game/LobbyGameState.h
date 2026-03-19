// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/Enum/StateEnums.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	void ChangeState(ELobbyState InState);

	FORCEINLINE ELobbyState GetState() const { return State; }
private:
	ELobbyState State = ELobbyState::Init;
};
