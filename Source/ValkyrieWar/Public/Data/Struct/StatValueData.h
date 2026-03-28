// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatValueData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStatValueData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float Attack = 0;
	UPROPERTY()
	float Defence = 0;
	UPROPERTY()
	float Health = 0;

public:
	void Add(float InAttack, float InDefence, float InHealth)
	{
		Attack += InAttack;
		Defence += InDefence;
		Health += InHealth;
	}

	void Add(FStatValueData InData)
	{
		Attack += InData.Attack;
		Defence += InData.Defence;
		Health += InData.Health;
	}
};
