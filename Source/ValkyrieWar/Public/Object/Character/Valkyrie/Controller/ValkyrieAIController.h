// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ValkyrieAIController.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AValkyrieAIController : public AAIController
{
	GENERATED_BODY()

public:
	AValkyrieAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
	float AISightRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
	float AILoseSightRadius = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
	float AISightAngle = 180.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBlackboardComponent> BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComp;

	//에디터에서 할당
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BTAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI") // 감각
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
