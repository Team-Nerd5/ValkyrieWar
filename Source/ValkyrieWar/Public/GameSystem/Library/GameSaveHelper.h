// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/Enum/DataEnums.h"
#include "Kismet/GameplayStatics.h"
#include "GameSaveHelper.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGameSaveHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static USaveGame* MakeSaveGame(ESaveType InType);
};
