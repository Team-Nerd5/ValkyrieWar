// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestCharacters/TestController/TestBaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

ATestBaseAIController::ATestBaseAIController()
{
    bAttachToPawn = true;
}

void ATestBaseAIController::ClearBBKeySafe(UBlackboardComponent* BB, const FName Key)
{
    if (BB && !Key.IsNone())
    {
        BB->ClearValue(Key);
    }
}

void ATestBaseAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}
