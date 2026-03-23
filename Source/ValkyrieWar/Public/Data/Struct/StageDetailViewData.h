#pragma once

#include "CoreMinimal.h"
#include "Data/Struct/StageEnemyViewData.h"
#include "RewardViewData.h"
#include "StageDetailViewData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStageDetailViewData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32 ChapterNum = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 StageNum = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 EnemyLevel = 1;

	UPROPERTY(BlueprintReadOnly)
	TArray<FStageEnemyViewData> Enemies;

	UPROPERTY(BlueprintReadOnly)
	TArray<FRewardViewData> Rewards;
};
