// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TestAttackReservedTarget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBTTask_TestAttackReservedTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_TestAttackReservedTarget();

    UPROPERTY(EditAnywhere, Category = "BB")
    struct FBlackboardKeySelector ReservedTargetKey;

    UPROPERTY(VisibleAnywhere, Category = "BB")
    struct FBlackboardKeySelector AttackTryCountKey;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
