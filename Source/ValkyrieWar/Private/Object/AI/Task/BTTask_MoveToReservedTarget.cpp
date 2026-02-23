// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/AI/Task/BTTask_MoveToReservedTarget.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveToReservedTarget::UBTTask_MoveToReservedTarget()
{
	NodeName = TEXT("Move To Reserved Target Or Base");
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MoveToReservedTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
            if (const AUnitCharacter* Unit = Cast<AUnitCharacter>(Pawn))
            {
                Radius = FMath::Max(AcceptableRadius, Unit->GetAttackRange() * RadiusScale);
            }
        }
    }

    // 성공/실패를 "나중에" 받기 위해 메시지 대기
    WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished);
    WaitForMessage(OwnerComp, UBrainComponent::AIMessage_RepathFailed);

    EPathFollowingRequestResult::Type Req = AIC->MoveToActor(
        Goal,
        Radius,
        bStopOnOverlap,
        true,
        true,
        nullptr,
        false
    );

    if (Req == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress;
}

void UBTTask_MoveToReservedTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    // 태스크가 중단/완료될 때 이동 정리
    if (TaskResult == EBTNodeResult::Aborted)
    {
        if (AAIController* AIC = OwnerComp.GetAIOwner())
        {
            AIC->StopMovement();
        }
    }

    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
