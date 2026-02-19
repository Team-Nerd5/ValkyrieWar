// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AIController.h"
#include "BTTask_TestMoveToReservedTarget.generated.h"

UCLASS()
class VALKYRIEWAR_API UBTTask_TestMoveToReservedTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_TestMoveToReservedTarget();

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
