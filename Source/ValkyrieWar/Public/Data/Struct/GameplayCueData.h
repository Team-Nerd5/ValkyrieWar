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
	UPROPERTY(EditAnywhere)
	EGameplayCueOrder CueOrder = EGameplayCueOrder::OnNotify;
	UPROPERTY(EditAnywhere)
	FGameplayTag Tag;
	UPROPERTY(EditAnywhere)
	FVector Offset = FVector::ZeroVector;
};
