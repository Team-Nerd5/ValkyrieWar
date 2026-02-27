// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EscapeStep.generated.h"

/**
 * EscapeStep Task
 *
 * 군중 전투 중 정체(Stuck) 상황이 감지되었을 때
 * 목표 방향 기준으로 짧은 SideStep 이동을 수행하는 보정 태스크.
 *
 * 중앙 BattleDirector의 전투 예약 구조는 유지한 채,
 * 국소 충돌/밀집으로 인한 이동 교착을 해소한다.
 *
 * 실행 플래그가 되는 BB 값은 UnitCharacter에서 정체상황을 감지하여 bool 값으로 셋팅해준다.
 */
UCLASS()
class VALKYRIEWAR_API UBTTask_EscapeStep : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_EscapeStep();

	// ===== BB Keys =====
	UPROPERTY(EditAnywhere, Category = "BB")
	FBlackboardKeySelector NeedToEscapeKey;

	// ReservedTarget/Base가 있으면 그 방향 기준으로 sidestep (없으면 컨트롤러 방향)
	UPROPERTY(EditAnywhere, Category = "BB")
	FBlackboardKeySelector ReservedTargetKey;

	UPROPERTY(EditAnywhere, Category = "BB")
	FBlackboardKeySelector EnemyBaseKey;

	// ===== Escape tuning =====
	UPROPERTY(EditAnywhere, Category = "Escape")
	float StepDistance = 400.f;

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
	FVector ComputeEscapeGoal(class AAIController* AIC, class UBlackboardComponent* BB, bool bTryOpposite) const;
	bool ProjectToNav(UWorld* World, const FVector& In, FVector& Out) const;

	void ClearNeedToEscape(UBehaviorTreeComponent& OwnerComp) const;
};
