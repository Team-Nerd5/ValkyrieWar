// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "ValkyrieDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FValkyrieDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	FString Name;
	UPROPERTY(EditAnywhere)
	EGradeType BaseGrade = EGradeType::Common;
	UPROPERTY(EditAnywhere)
	int32 StatId = 0;
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100.0f;
	UPROPERTY(EditAnywhere)
	int32 BaseWeaponId = 0;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class AValkyrieCharacter> SpawnClass = nullptr;
};
