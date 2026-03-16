// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/Table/GameData/GachaRandomDataRow.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "RandomGenerateHelper.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API URandomGenerateHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION()
	static int32 GetRandomValkyrie(UDataManager* InDataManager, int32 InGroupId);

private:
	static bool IsGachaDataValid(TMap<int32, FGachaRandomDataRow> InData);
};
