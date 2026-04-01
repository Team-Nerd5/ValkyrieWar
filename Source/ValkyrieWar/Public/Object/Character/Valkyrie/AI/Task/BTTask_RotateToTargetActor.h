#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToTargetActor.generated.h"

UCLASS()
class VALKYRIEWAR_API UBTTask_RotateToTargetActor : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RotateToTargetActor();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 타겟 Actor 키
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetActorKey;

	// 회전 속도
	UPROPERTY(EditAnywhere, Category = "Rotate")
	float RotationSpeed = 5.0f;

	// 완료 판정 각도
	UPROPERTY(EditAnywhere, Category = "Rotate")
	float AcceptableAngle = 5.0f;
};
