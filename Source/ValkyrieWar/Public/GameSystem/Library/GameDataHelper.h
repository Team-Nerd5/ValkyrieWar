// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/Enum/DataEnums.h"
#include "GameDataHelper.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGameDataHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION()
	static EItemGroup GetItemGroup(EItemType InItemType);

	UFUNCTION()
	static EEquipGroup GetEquipGroup(EItemType InItemType);
};
