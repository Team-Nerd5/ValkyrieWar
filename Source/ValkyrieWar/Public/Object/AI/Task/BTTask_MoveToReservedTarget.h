// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToReservedTarget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBTTask_MoveToReservedTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_MoveToReservedTarget();

    UPROPERTY(EditAnywhere, Category = "BB")
    FBlackboardKeySelector ReservedTargetKey;

    UPROPERTY(EditAnywhere, Category = "BB")
    FBlackboardKeySelector EnemyBaseKey;

    UPROPERTY(EditAnywhere, Category = "Move")
    float AcceptableRadius = 100.f;

    UPROPERTY(EditAnywhere, Category = "Move")
    bool bUseUnitAttackRangeAsRadius = true;

    UPROPERTY(EditAnywhere, Category = "Move")
    float RadiusScale = 0.80f;

    UPROPERTY(EditAnywhere, Category = "Move")
    bool bStopOnOverlap = true;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};
