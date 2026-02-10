// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/ValkyrieDataRow.h"
#include "ValkyrieData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieData : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	uint64 UID = 0;

	const FValkyrieDataRow* TableData = nullptr;
};
