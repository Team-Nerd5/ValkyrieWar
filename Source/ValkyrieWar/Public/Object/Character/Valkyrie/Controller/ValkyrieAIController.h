#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ValkyrieAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UBattleDirectorSubsystem;

UCLASS()
class VALKYRIEWAR_API AValkyrieAIController : public AAIController
{
	GENERATED_BODY()

public:
	AValkyrieAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BlackboardComp = nullptr;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp = nullptr;

	// 에디터에서 할당
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BTAsset = nullptr;

	// 발키리가 "근처 적"을 판단할 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Target")
	float EnemyDetectRadius = 700.0f;

	// 블랙보드 키 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Blackboard")
	FName TargetActorKeyName = TEXT("TargetActor");

	UPROPERTY()
	TObjectPtr<UBattleDirectorSubsystem> BattleDirector = nullptr;
};
