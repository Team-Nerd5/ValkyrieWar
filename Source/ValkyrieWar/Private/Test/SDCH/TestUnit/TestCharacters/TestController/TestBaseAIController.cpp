// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestCharacters/TestController/TestBaseAIController.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

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

    if (!BehaviorTreeAsset) return;

    UBlackboardData* BBAsset = BehaviorTreeAsset->BlackboardAsset;
    if (!BBAsset) return;

    UBlackboardComponent* BB = nullptr;
    if (UseBlackboard(BBAsset, BB))
    {
        ATestBaseUnit* Me = Cast<ATestBaseUnit>(InPawn);

        BB->SetValueAsFloat(TEXT("MoveAcceptRad"), Me->AttackRange * 0.8f);

        RunBehaviorTree(BehaviorTreeAsset);
    }
}
