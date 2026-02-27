// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateReservation.generated.h"

/**
 * UpdateReservation Service
 *
 * 군중 전투에서 각 유닛의 전투 타깃 예약을
 * BattleDirectorSubsystem을 통해 갱신하는 서비스.
 *
 * 중앙에서 결정된 ReservedTarget과 EnemyBase를
 * Blackboard에 동기화하여 BT 흐름을 안정적으로 유지한다.
 */
UCLASS()
class VALKYRIEWAR_API UBTService_UpdateReservation : public UBTService
{
	GENERATED_BODY()

public:
    UBTService_UpdateReservation();

    // Blackboard keys
    UPROPERTY(EditAnywhere, Category = "BB")
    struct FBlackboardKeySelector ReservedTargetKey;

    UPROPERTY(EditAnywhere, Category = "BB")
    struct FBlackboardKeySelector EnemyBaseKey;

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
