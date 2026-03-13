// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Enum/CommonEnums.h"
#include "ProjectileDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FProjectileDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1000.0f;
	UPROPERTY()
	EPoolTypes EPoolTypes = EPoolTypes::None;
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class ABaseProjectile> SpawnObject = nullptr;
	UPROPERTY(EditAnywhere)
	EProjectileShootType ShootType = EProjectileShootType::None;
	UPROPERTY(EditAnywhere)
	int32 Amount = 1;
};
