// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TestEscapeStep.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBTTask_TestEscapeStep : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TestEscapeStep();

	// ===== BB Keys =====
	UPROPERTY(EditAnywhere, Category = "BB")
	FBlackboardKeySelector NeedToEscapeKey; // bool

	// 선택: ReservedTarget/Base가 있으면 그 방향 기준으로 sidestep (없으면 컨트롤러 방향)
	UPROPERTY(EditAnywhere, Category = "BB")
	FBlackboardKeySelector ReservedTargetKey; // Object (Actor)

	UPROPERTY(EditAnywhere, Category = "BB")
	FBlackboardKeySelector EnemyBaseKey; // Object (Actor)

	// (선택) 디버그/관측용으로 이번에 선택한 탈출 목표를 BB에 저장하고 싶다면
	//UPROPERTY(EditAnywhere, Category = "BB")
	//FBlackboardKeySelector EscapeGoalKey; // Vector (optional)

	// ===== Escape tuning =====
	UPROPERTY(EditAnywhere, Category = "Escape")
	float StepDistance = 400.f; // cm

	UPROPERTY(EditAnywhere, Category = "Escape")
	float AcceptableRadius = 30.f;

	// 좌/우 랜덤이 아니라, 목표 방향 기준으로 "좌/우"를 잡고 싶을 때
	UPROPERTY(EditAnywhere, Category = "Escape")
	bool bUseGoalBasedSideStep = true;

	// 목표 방향이 없을 때, 컨트롤러 yaw 기준으로 sidestep
	UPROPERTY(EditAnywhere, Category = "Escape")
	bool bUseControllerYawFallback = true;

	// 네비 투영 반경
	UPROPERTY(EditAnywhere, Category = "Escape")
	float NavProjectExtentXY = 120.f;

	UPROPERTY(EditAnywhere, Category = "Escape")
	float NavProjectExtentZ = 200.f;

	// 실패했을 때 반대 방향으로 1회 재시도
	UPROPERTY(EditAnywhere, Category = "Escape")
	bool bRetryOppositeIfNavProjectFails = true;

	// 태스크 종료 시 NeedToEscape를 항상 내려줌
	UPROPERTY(EditAnywhere, Category = "Escape")
	bool bClearNeedToEscapeOnFinish = true;

	// (옵션) 유닛의 ResetStuckCountdown 호출
	UPROPERTY(EditAnywhere, Category = "Escape")
	bool bResetUnitStuckCountdown = true;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	FVector ComputeEscapeGoal(class AAIController* AIC, class UBlackboardComponent* BB, bool bTryOpposite, bool& bOutFoundBaseDir) const;
	bool ProjectToNav(UWorld* World, const FVector& In, FVector& Out) const;

	void ClearNeedToEscape(UBehaviorTreeComponent& OwnerComp) const;
};
