// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GoodsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGoodsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint64 Ticket;

	UPROPERTY()
	uint64 Gem;

	UPROPERTY()
	uint64 Gold;
};
