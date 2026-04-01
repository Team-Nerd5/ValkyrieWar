#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToCombatRange.generated.h"

UCLASS()
class VALKYRIEWAR_API UBTTask_MoveToCombatRange : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToCombatRange();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CombatRangeKey;

	UPROPERTY(EditAnywhere, Category = "Move")
	float AcceptanceBuffer = .1f;

	UPROPERTY(EditAnywhere, Category = "Move")
	bool bAllowPartialPath = false;
};
