// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FUnitDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId;
	UPROPERTY(EditAnywhere)
	int32 StatId;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> Mesh = nullptr;
	UPROPERTY(EditAnywhere)
	float MovSpeed;
	UPROPERTY(EditAnywhere)
	int32 BaseWeaponId;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AUnitCharacter> SpawnClass = nullptr;
};
