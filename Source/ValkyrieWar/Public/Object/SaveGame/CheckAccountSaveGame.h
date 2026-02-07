// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CheckAccountSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCheckAccountSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint64 UserId = 0;
};
