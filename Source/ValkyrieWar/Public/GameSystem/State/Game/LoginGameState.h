// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/Enums.h"
#include "LoginGameState.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ALoginGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void ChangeGameState(ELoginState InState);

private:
	ELoginState State = ELoginState::Init;
};
