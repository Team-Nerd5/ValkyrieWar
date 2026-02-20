// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/AI/Task/BTTask_AttackReservedTarget.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AttackReservedTarget::UBTTask_AttackReservedTarget()
{
	NodeName = TEXT("Attack Reserved Target");
}

EBTNodeResult::Type UBTTask_AttackReservedTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (!AIC || !BB) return EBTNodeResult::Failed;

    AUnitCharacter* Unit = Cast<AUnitCharacter>(AIC->GetPawn());
    if (!Unit) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(ReservedTargetKey.SelectedKeyName));
    if (!Target) return EBTNodeResult::Failed;

    const FVector MyLoc = Unit->GetActorLocation();
    const FVector TargetLoc = Target->GetActorLocation();

    int32 TryCount = BB->GetValueAsInt(AttackTryCountKey.SelectedKeyName);

    if (TryCount >= 50)
    {
        if (UBattleDirectorSubsystem* Subsys = GetWorld()->GetSubsystem<UBattleDirectorSubsystem>())
        {
            BB->SetValueAsInt(AttackTryCountKey.SelectedKeyName, 0);
            AUnitCharacter* CurrentTarget = Cast<AUnitCharacter>(Target);
            Subsys->ReleaseReservation(Unit, CurrentTarget);
            return EBTNodeResult::Failed;
        }
    }

    const float Dist2D = FVector::Dist2D(MyLoc, TargetLoc);

    // 사거리 경계면 재접근
    if (Dist2D > Unit->AttackRange * 1.4)
    {
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
