// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TestUpdateReservation.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBTService_TestUpdateReservation : public UBTService
{
	GENERATED_BODY()

public:
    UBTService_TestUpdateReservation();

    // Blackboard keys
    UPROPERTY(EditAnywhere, Category = "BB")
    struct FBlackboardKeySelector ReservedTargetKey;

    UPROPERTY(EditAnywhere, Category = "BB")
    struct FBlackboardKeySelector EnemyBaseKey;

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
