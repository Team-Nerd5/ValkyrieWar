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

	UFUNCTION()
	static TArray<int32> GetStageRewards(TArray<FStageRewardDataRow> InRewardData);

	UFUNCTION()
	static int32 GetRandomValkyrieInGrade(UDataManager* InDataManager, EGradeType InGrade);
private:
	static bool IsGachaDataValid(TMap<EGradeType, FGachaRandomDataRow> InData);

};
