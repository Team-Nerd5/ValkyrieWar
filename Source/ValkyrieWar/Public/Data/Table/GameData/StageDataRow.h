// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StageDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStageDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere)
	int32 MissionId1 = 0;
	UPROPERTY(EditAnywhere)
	int32 MissionId2 = 0;
	UPROPERTY(EditAnywhere)
	int32 MissionId3 = 0;
	UPROPERTY(EditAnywhere)
	int32 StageGroupId = 0;
};
