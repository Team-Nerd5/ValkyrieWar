// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestCharacters/TestController/TestBaseAIController.h"
//#include "BehaviorTree/BehaviorTree.h"
//#include "BehaviorTree/BlackboardComponent.h"

ATestBaseAIController::ATestBaseAIController()
{
    bAttachToPawn = true;
}

void ATestBaseAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}
