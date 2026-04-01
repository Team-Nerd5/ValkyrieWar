#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackValkyrieTarget.generated.h"

UCLASS()
class VALKYRIEWAR_API UBTTask_AttackValkyrieTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AttackValkyrieTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// 사거리 판정 여유 배수
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRangeMultiplier = 1.2f;
};
