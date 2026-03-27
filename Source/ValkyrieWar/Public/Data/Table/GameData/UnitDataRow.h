// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/CommonEnums.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Enum/DataEnums.h"
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
	EGradeType Grade = EGradeType::Common;
	UPROPERTY(EditAnywhere)
	float MovSpeed = 0.0f;
	UPROPERTY(EditAnywhere)
	int32 BaseWeaponId = 0;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AUnitCharacter> SpawnClass = nullptr;
	UPROPERTY(EditAnywhere)
	ETeamType TeamType = ETeamType::None;
	UPROPERTY(EditAnywhere)
	EPoolTypes PoolType = EPoolTypes::None;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> UnitIcon = nullptr;
	UPROPERTY(EditAnywhere)
	EUnitCharacterType UnitType = EUnitCharacterType::SwordMan;
	UPROPERTY(EditAnywhere)
	int32 LevelUpGroupId = 0;
	UPROPERTY(EditAnywhere)
	float SpawnCooltime = 0.0f;
};
