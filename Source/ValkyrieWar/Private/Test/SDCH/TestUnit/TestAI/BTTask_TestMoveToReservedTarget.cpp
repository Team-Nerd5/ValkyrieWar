#include "Test/SDCH/TestUnit/TestAI/BTTask_TestMoveToReservedTarget.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_TestMoveToReservedTarget::UBTTask_TestMoveToReservedTarget()
{
    NodeName = TEXT("Test Move To Reserved Target Or Base");
    bNotifyTaskFinished = true; // OnTaskFinished 사용
}

EBTNodeResult::Type UBTTask_TestMoveToReservedTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AIC || !BB) return EBTNodeResult::Failed;

    AActor* Reserved = Cast<AActor>(BB->GetValueAsObject(ReservedTargetKey.SelectedKeyName));
    AActor* Base     = Cast<AActor>(BB->GetValueAsObject(EnemyBaseKey.SelectedKeyName));
    AActor* Goal     = Reserved ? Reserved : Base;
    if (!Goal) return EBTNodeResult::Failed;

    float Radius = AcceptableRadius;

    if (bUseUnitAttackRangeAsRadius)
    {
        if (const APawn* Pawn = AIC->GetPawn())
        {
            if (const ATestBaseUnit* Unit = Cast<ATestBaseUnit>(Pawn))
            {
                Radius = FMath::Max(AcceptableRadius, Unit->AttackRange * RadiusScale);
            }
        }
    }

    // 핵심: 성공/실패를 "나중에" 받기 위해 메시지 대기
    // UE 기본 메시지: AIMessage_MoveFinished, AIMessage_RepathFailed
    WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished);
    WaitForMessage(OwnerComp, UBrainComponent::AIMessage_RepathFailed);

    EPathFollowingRequestResult::Type Req = AIC->MoveToActor(
        Goal,
        Radius,
        bStopOnOverlap,
        true,   // bUsePathfinding
        true,   // bCanStrafe
        nullptr,
        false
    );

    if (Req == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress;
}

void UBTTask_TestMoveToReservedTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
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
