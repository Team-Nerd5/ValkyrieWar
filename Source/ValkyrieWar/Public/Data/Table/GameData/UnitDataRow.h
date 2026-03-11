// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/CommonEnums.h"
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
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	int32 StatId = 0;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> Mesh = nullptr;
	UPROPERTY(EditAnywhere)
	float MovSpeed = 0.0f;
	UPROPERTY(EditAnywhere)
	int32 BaseWeaponId = 0;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AUnitCharacter> SpawnClass = nullptr;
	UPROPERTY(EditAnywhere)
	ETeamType TeamType = ETeamType::None;
};
