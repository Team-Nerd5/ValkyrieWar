// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestAI/BTTask_TestAttackReservedTarget.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "Test/SDCH/TestGameSystem/TestInstance/TestWorld/TestBattleDirectorSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_TestAttackReservedTarget::UBTTask_TestAttackReservedTarget()
{
    NodeName = TEXT("Test Attack Reserved Target");
}

EBTNodeResult::Type UBTTask_TestAttackReservedTarget::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (!AIC || !BB) return EBTNodeResult::Failed;

    ATestBaseUnit* Unit = Cast<ATestBaseUnit>(AIC->GetPawn());
    if (!Unit) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(ReservedTargetKey.SelectedKeyName));
    if (!Target) return EBTNodeResult::Failed;

    const FVector MyLoc = Unit->GetActorLocation();
    const FVector TargetLoc = Target->GetActorLocation();

    int32 TryCount = BB->GetValueAsInt(AttackTryCountKey.SelectedKeyName);

    if (TryCount >= 150)
    {
        if (UTestBattleDirectorSubsystem* Subsys = GetWorld()->GetSubsystem<UTestBattleDirectorSubsystem>())
        {
            BB->SetValueAsInt(AttackTryCountKey.SelectedKeyName, 0);
            ATestBaseUnit* CurrentTarget = Cast<ATestBaseUnit>(Target);
            Subsys->ReleaseReservation(Unit, CurrentTarget);
            return EBTNodeResult::Failed;
        }
    }

    // 2D 거리 사용 (경사/단차 안정성)
    const float Dist2D = FVector::Dist2D(MyLoc, TargetLoc);

    //if (Dist2D > Unit->AttackRange + 30.f)
    //{
    //    TryCount += 1;
    //    BB->SetValueAsInt(AttackTryCountKey.SelectedKeyName, TryCount);
    //    return EBTNodeResult::Failed;
    //}

    // 사거리 경계면 재접근
    if (Dist2D > Unit->AttackRange)
    {
        AIC->MoveToActor(Target, Unit->AttackRange * 0.85f);
        TryCount += 1;
        BB->SetValueAsInt(AttackTryCountKey.SelectedKeyName, TryCount);
        return EBTNodeResult::Failed;
    }

    // ============================
    // 공격 실행
    // ============================
    BB->SetValueAsInt(AttackTryCountKey.SelectedKeyName, 0);

    Unit->PerformAttack(Target);
    return EBTNodeResult::Succeeded;
}
