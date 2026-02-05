// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAccountData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FPlayerAccountData
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString PlayerName = TEXT("Player");

    UPROPERTY()
    int32 PlayerLevel = 1;

    UPROPERTY()
    int32 PlayerExp = 0;

};
