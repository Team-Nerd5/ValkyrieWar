// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Data/Enum/CommonEnums.h"
#include "ValkyriePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AValkyriePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AValkyriePlayerState();

	EInputControlMode GetControlMode() const { return CurrentControlMode; }

	void ToggleControlMode();

protected:
	void SetControlMode(EInputControlMode NewMode);

private:
	UPROPERTY(VisibleAnywhere, Category = "Battle")
	EInputControlMode CurrentControlMode = EInputControlMode::Manual;
};
