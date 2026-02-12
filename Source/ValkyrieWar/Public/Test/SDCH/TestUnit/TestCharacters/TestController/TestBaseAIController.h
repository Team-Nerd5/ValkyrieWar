// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "TestBaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ATestBaseAIController : public ADetourCrowdAIController
{
	GENERATED_BODY()

public:
    ATestBaseAIController();

    void ClearBBKeySafe(UBlackboardComponent* BB, const FName Key);

    UPROPERTY(EditAnywhere, Category = "AI|BT")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

protected:
    virtual void OnPossess(APawn* InPawn) override;
};
