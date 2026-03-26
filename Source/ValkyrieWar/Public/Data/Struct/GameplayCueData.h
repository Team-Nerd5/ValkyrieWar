// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Data/Enum/CharacterEnums.h"
#include "GameplayCueData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FGameplayCueData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	EGameplayCueOrder CueOrder = EGameplayCueOrder::OnNotify;
	UPROPERTY()
	FGameplayTag Tag;
	UPROPERTY()
	FVector Offset = FVector::ZeroVector;
};
