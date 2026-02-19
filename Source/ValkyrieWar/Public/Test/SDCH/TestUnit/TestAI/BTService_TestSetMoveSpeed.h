// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TestSetMoveSpeed.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBTService_TestSetMoveSpeed : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Speed")
	float WalkSpeed = 150.f;


	UPROPERTY(EditAnywhere, Category = "Speed")
	float RunSpeed = 600.f;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey; // TargetActor


protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
