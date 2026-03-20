#pragma once

#include "CoreMinimal.h"
#include "Data/Table/GameData/StageRewardDataRow.h"
#include "StageRewardArray.generated.h"

USTRUCT()
struct FStageRewardArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FStageRewardDataRow> Rows;
};
