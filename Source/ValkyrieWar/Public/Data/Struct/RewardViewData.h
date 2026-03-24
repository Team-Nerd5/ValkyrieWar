#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "RewardViewData.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FRewardViewData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ERewardType RewardType = ERewardType::None;

	UPROPERTY(BlueprintReadOnly)
	int32 RewardId = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 RewardDataId = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Amount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 RewardRate = 0; // 만분율

	UPROPERTY(BlueprintReadOnly)
	FString Name = TEXT("");

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
};
