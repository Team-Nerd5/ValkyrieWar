// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FPlayerSaveData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    FString PlayerName = TEXT("Player");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
    int32 Gold = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
    int32 Ticket = 0;

};
