#pragma once

#include "CoreMinimal.h"
#include "ComputedEnemyStat.generated.h"


USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FComputedEnemyStat
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 DataId = 0;

	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	float Attack = 0.f;

	UPROPERTY()
	float Defence = 0.f;

	UPROPERTY()
	float Health = 0.f;
};
