// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/CharacterEnums.h"
#include "EquipTransformDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FEquipTransformDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 ValkyrieId = 0;
	UPROPERTY(EditAnywhere)
	int32 TransformGroupId = 0;
	UPROPERTY(EditAnywhere)
	int32 ItemId = 0;
	UPROPERTY(EditAnywhere)
	FVector LocationtionOffset = FVector::ZeroVector;		//무기 포지션 오프셋
	UPROPERTY(EditAnywhere)
	FRotator RotateOffset = FRotator::ZeroRotator;
};
