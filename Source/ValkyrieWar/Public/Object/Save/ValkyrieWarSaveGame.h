// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Save/PlayerAccountData.h"
#include "ValkyrieWarSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieWarSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FPlayerAccountData PlayerAccountData;

};
