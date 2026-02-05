// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/Enums.h"
#include "BattleGameState.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ABattleGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void ChangeState(EBattleState InState);

	FORCEINLINE EBattleState GetState() const { return State; }
	FORCEINLINE bool IsPlaying() const { return State == EBattleState::Play; }

private:
	EBattleState State = EBattleState::Init;
};
