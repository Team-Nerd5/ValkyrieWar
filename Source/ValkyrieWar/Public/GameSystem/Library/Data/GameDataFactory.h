// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/Game/ValkyrieData.h"
#include "GameDataFactory.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGameDataFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UValkyrieData* GenerateValkyrie(int32 InDataId, UGameInstance* InGameInstance);
};
