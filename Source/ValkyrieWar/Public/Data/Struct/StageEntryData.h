#pragma once

#include "CoreMinimal.h"
#include "StageEntryData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStageEntryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Chapter = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StageNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCleared = false;
};
