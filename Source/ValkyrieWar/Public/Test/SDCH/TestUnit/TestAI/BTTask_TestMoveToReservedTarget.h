// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TestMoveToReservedTarget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBTTask_TestMoveToReservedTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_TestMoveToReservedTarget();

    UPROPERTY(EditAnywhere, Category = "BB")
    struct FBlackboardKeySelector ReservedTargetKey;

    UPROPERTY(EditAnywhere, Category = "BB")
    struct FBlackboardKeySelector EnemyBaseKey;

    UPROPERTY(EditAnywhere)
    float AcceptableRadius = 80.f;

    UPROPERTY(EditAnywhere)
    bool bUseUnitAttackRangeAsRadius = true;

    UPROPERTY(EditAnywhere)
    float RadiusScale = 0.9f;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
