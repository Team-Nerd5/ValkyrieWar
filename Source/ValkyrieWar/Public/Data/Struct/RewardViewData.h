#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "RewardViewData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FRewardViewData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EItemType ItemType = EItemType::None;

	UPROPERTY(BlueprintReadOnly)
	int32 DataId = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Amount = 0;

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;
};
