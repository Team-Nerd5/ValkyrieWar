// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AccountSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UAccountSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString Nickname;

	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	int32 Exp = 0;

};
