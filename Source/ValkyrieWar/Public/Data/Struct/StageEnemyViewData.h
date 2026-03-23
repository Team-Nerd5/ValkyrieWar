#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/CommonEnums.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Enum/DataEnums.h"
#include "StageEnemyViewData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FStageEnemyViewData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32 UnitId = 0;

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> UnitIcon = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EGradeType Grade = EGradeType::Common;

	UPROPERTY(BlueprintReadOnly)
	EUnitCharacterType UnitType = EUnitCharacterType::SwordMan;

	UPROPERTY(BlueprintReadOnly)
	ETeamType TeamType = ETeamType::None;

	UPROPERTY(BlueprintReadOnly)
	int32 Level = 1;
};
