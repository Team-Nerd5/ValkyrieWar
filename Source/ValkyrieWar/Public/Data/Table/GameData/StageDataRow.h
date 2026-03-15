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
	//챕터별 데이터
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	int32 Chapter = 0;
	UPROPERTY(EditAnywhere)
	int32 ChapterMissionId = 0;
	UPROPERTY(EditAnywhere)
	int32 StageNum = 0;
};
