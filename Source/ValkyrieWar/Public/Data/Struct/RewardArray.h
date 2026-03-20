#pragma once

#include "CoreMinimal.h"
#include "Data/Table/GameData/RewardDataRow.h"
#include "RewardArray.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FRewardArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FRewardDataRow> Rows;
};
