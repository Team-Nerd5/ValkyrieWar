// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Unit/AI/Controller/UnitAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AUnitAIController::AUnitAIController()
{
    bAttachToPawn = true;
}

void AUnitAIController::ClearBBKeySafe(UBlackboardComponent* BB, const FName Key)
{
    if (BB && !Key.IsNone())
    {
        BB->ClearValue(Key);
    }
}

void AUnitAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset) RunBehaviorTree(BehaviorTreeAsset);
}
