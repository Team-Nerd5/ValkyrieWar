// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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

	virtual void  OnPossess(APawn* InPawn) override;
protected:
	UPROPERTY()
	TObjectPtr<class UBlackboardComponent> BlackboardComp;
	UPROPERTY()
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComp;

	//에디터에서 할당
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BTAsset;
};
