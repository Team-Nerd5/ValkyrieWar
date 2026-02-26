// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Game/ValkyrieData.h"
#include "ValkyrieSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieSaveGame : public USaveGame
{
	GENERATED_BODY()

	//uid 키로 성장요소랑 음...
public:
	UPROPERTY()
	TMap<int64, UValkyrieData*> ValkyrieData;
};
