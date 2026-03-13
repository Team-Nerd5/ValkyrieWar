// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/Enum/StateEnums.h"
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

	// ===== Stage Timer API =====
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void StartStageTimer(float InLimitSeconds);

	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void StopStageTimer();

	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	float GetRemainingSeconds() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	float GetLimitSeconds() const { return TimeLimitSeconds; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void PlayGame();
	void CheckTimeOver();
	void ShowBattleResult();

private:
	EBattleState State = EBattleState::Init;

	int32 InitialMana = 50;

	// ===== Timer Data =====
	UPROPERTY(EditAnywhere, Category = "Battle|Timer")
	float TimeLimitSeconds = 180.f; // 기본 3분

	bool bTimerRunning = false;
	double EndTimeSeconds = 0.0;

	FTimerHandle TimerCheckHandle;
};
