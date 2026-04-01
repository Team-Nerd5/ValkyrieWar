#include "Object/Character/Valkyrie/AI/Task/BTTask_MoveToCombatRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveToCombatRange::UBTTask_MoveToCombatRange()
{
	NodeName = TEXT("Move To Combat Range");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MoveToCombatRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIC || !BB)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}

	const float CombatRange = BB->GetValueAsFloat(CombatRangeKey.SelectedKeyName);
	if (CombatRange <= 0.0f)
	{
		return EBTNodeResult::Failed;
	}

	const float Dist2D = FVector::Dist2D(Pawn->GetActorLocation(), Target->GetActorLocation());
	if (Dist2D <= CombatRange)
	{
		return EBTNodeResult::Succeeded;
	}

	FAIMoveRequest MoveReq;
	MoveReq.SetGoalActor(Target);
	MoveReq.SetAcceptanceRadius(FMath::Max(10.0f, CombatRange * AcceptanceBuffer));
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(bAllowPartialPath);

	FNavPathSharedPtr NavPath;
	const EPathFollowingRequestResult::Type MoveResult = AIC->MoveTo(MoveReq, &NavPath);

	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToCombatRange::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIC || !BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float CombatRange = BB->GetValueAsFloat(CombatRangeKey.SelectedKeyName);
	if (CombatRange <= 0.0f)
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float Dist2D = FVector::Dist2D(Pawn->GetActorLocation(), Target->GetActorLocation());

	// 실제 전투 가능 거리 안에 들어오면 성공
	if (Dist2D <= CombatRange)
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 이동이 끊겼거나 실패했는데 아직 사거리 밖이면 실패 처리
	UPathFollowingComponent* PathComp = AIC->GetPathFollowingComponent();
	if (!PathComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const EPathFollowingStatus::Type MoveStatus = PathComp->GetStatus();
	if (MoveStatus != EPathFollowingStatus::Moving && MoveStatus != EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (MoveStatus == EPathFollowingStatus::Idle)
	{
		if (Dist2D <= CombatRange)
		{
			AIC->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			AIC->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
		return;
	}
}

EBTNodeResult::Type UBTTask_MoveToCombatRange::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (AIC)
	{
		AIC->StopMovement();
	}

	return EBTNodeResult::Aborted;
}
