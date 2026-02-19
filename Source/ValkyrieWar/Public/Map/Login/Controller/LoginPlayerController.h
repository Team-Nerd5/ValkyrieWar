// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/Enum/StateEnums.h"
#include "LoginPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ALoginPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ChageGameState(ELoginState InState);
};
