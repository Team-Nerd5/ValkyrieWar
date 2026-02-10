// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GachaSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGachaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 CeilValue = 0;

	UPROPERTY()
	uint64 GachaCount = 0;
};
