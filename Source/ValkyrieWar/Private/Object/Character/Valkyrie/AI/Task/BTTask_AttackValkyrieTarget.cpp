#include "Object/Character/Valkyrie/AI/Task/BTTask_AttackValkyrieTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

UBTTask_AttackValkyrieTarget::UBTTask_AttackValkyrieTarget()
{
	NodeName = TEXT("Attack Valkyrie Target");
}

EBTNodeResult::Type UBTTask_AttackValkyrieTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIC || !BB)
	{
		return EBTNodeResult::Failed;
	}

	AValkyrieCharacter* Valkyrie = Cast<AValkyrieCharacter>(AIC->GetPawn());
	if (!Valkyrie)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}

	const FVector MyLoc = Valkyrie->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();
	const float Dist2D = FVector::Dist2D(MyLoc, TargetLoc);

	const float AttackRange = Valkyrie->GetAttackRange();

	if (Dist2D > AttackRange * AttackRangeMultiplier)
	{
		return EBTNodeResult::Failed;
	}

	Valkyrie->UpdateTarget(Target);

	return EBTNodeResult::Succeeded;
}
