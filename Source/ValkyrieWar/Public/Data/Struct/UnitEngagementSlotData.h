#pragma once

#include "CoreMinimal.h"
#include "UnitEngagementSlotData.generated.h"

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FUnitEngagementSlotData
{
    GENERATED_BODY()

    // 슬롯에 들어온 공격자
    UPROPERTY()
    TWeakObjectPtr<AActor> Attacker;

    // 슬롯을 확보한 시간 (thrashing 방지/디버그 용)
    UPROPERTY()
    float ReservedAtTime = 0.f;

    bool IsValid() const { return Attacker.IsValid(); }
    void Reset() { Attacker.Reset(); ReservedAtTime = 0.f; }
};
