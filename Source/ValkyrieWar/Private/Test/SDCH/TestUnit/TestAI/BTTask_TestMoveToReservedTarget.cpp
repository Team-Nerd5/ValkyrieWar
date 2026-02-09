// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestAI/BTTask_TestMoveToReservedTarget.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_TestMoveToReservedTarget::UBTTask_TestMoveToReservedTarget()
{
    NodeName = TEXT("Test Move To Reserved Target Or Base");
}

EBTNodeResult::Type UBTTask_TestMoveToReservedTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AIC || !BB) return EBTNodeResult::Failed;

    AActor* Reserved = Cast<AActor>(BB->GetValueAsObject(ReservedTargetKey.SelectedKeyName));
    AActor* Base = Cast<AActor>(BB->GetValueAsObject(EnemyBaseKey.SelectedKeyName));

    AActor* Goal = Reserved ? Reserved : Base;

    if (!Goal) return EBTNodeResult::Failed;

    float Radius = AcceptableRadius;

    if (bUseUnitAttackRangeAsRadius)
    {
        if (const APawn* Pawn = AIC->GetPawn())
        {
            if (const ATestBaseUnit* Unit = Cast<ATestBaseUnit>(Pawn))
            {
                Radius = FMath::Max(80.f, Unit->AttackRange * RadiusScale);
            }
        }
    }

    const EPathFollowingRequestResult::Type R =
        AIC->MoveToActor(Goal, Radius, true, true, true, nullptr, true);
    return (R == EPathFollowingRequestResult::Failed) ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
}
