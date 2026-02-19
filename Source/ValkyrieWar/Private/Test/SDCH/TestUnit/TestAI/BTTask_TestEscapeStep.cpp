// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestAI/BTTask_TestEscapeStep.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"

#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"

UBTTask_TestEscapeStep::UBTTask_TestEscapeStep()
{
	NodeName = TEXT("Test Escape Step (SideStep)");
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_TestEscapeStep::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) return EBTNodeResult::Failed;

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	// NeedToEscape가 false인데 호출되면 그냥 성공 처리(트리 안정성)
	const bool bNeed = BB->GetValueAsBool(NeedToEscapeKey.SelectedKeyName);
	if (!bNeed)
		return EBTNodeResult::Succeeded;

	// 메시지 기반으로 종료
	WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished);
	WaitForMessage(OwnerComp, UBrainComponent::AIMessage_RepathFailed);

	bool bHasBaseDir = false;
	FVector EscapeGoal = ComputeEscapeGoal(AIC, BB, /*bTryOpposite=*/false, bHasBaseDir);

	// 네비 투영 실패하면 반대 방향 재시도(옵션)
	UWorld* World = OwnerComp.GetWorld();
	if (!World) return EBTNodeResult::Failed;

	FVector Projected = EscapeGoal;
	bool bProjectedOK = ProjectToNav(World, EscapeGoal, Projected);

	if (!bProjectedOK && bRetryOppositeIfNavProjectFails)
	{
		EscapeGoal = ComputeEscapeGoal(AIC, BB, /*bTryOpposite=*/true, bHasBaseDir);
		bProjectedOK = ProjectToNav(World, EscapeGoal, Projected);
	}

	if (!bProjectedOK)
	{
		// 투영 실패: 그냥 플래그 내리고 종료(무한 루프 방지)
		ClearNeedToEscape(OwnerComp);
		return EBTNodeResult::Failed;
	}

	// (선택) BB에 저장해서 디버그/관측
	//if (EscapeGoalKey.SelectedKeyName.IsNone() == false)
	//{
	//	BB->SetValueAsVector(EscapeGoalKey.SelectedKeyName, Projected);
	//}

	// 기존 이동 끊고 짧게 옆걸음
	AIC->StopMovement();

	const EPathFollowingRequestResult::Type Req = AIC->MoveToLocation(
		Projected,
		AcceptableRadius,
		/*bStopOnOverlap=*/true,
		/*bUsePathfinding=*/true,
		/*bProjectDestinationToNavigation=*/false, // 이미 project 함
		/*bCanStrafe=*/true,
		/*FilterClass=*/nullptr,
		/*bAllowPartialPath=*/true
	);

	if (Req == EPathFollowingRequestResult::Failed)
	{
		ClearNeedToEscape(OwnerComp);
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_TestEscapeStep::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	// 항상 플래그 내림(권장)
	if (bClearNeedToEscapeOnFinish)
	{
		ClearNeedToEscape(OwnerComp);
	}

	// (옵션) 유닛의 stuck 카운트다운 리셋
	if (bResetUnitStuckCountdown)
	{
		if (AAIController* AIC = OwnerComp.GetAIOwner())
		{
			if (ATestBaseUnit* Unit = Cast<ATestBaseUnit>(AIC->GetPawn()))
			{
				Unit->ResetStuckCountdown(/*bAlsoClearBB=*/false);
			}
		}
	}

	// 중단이면 이동 정리
	if (TaskResult == EBTNodeResult::Aborted)
	{
		if (AAIController* AIC = OwnerComp.GetAIOwner())
		{
			AIC->StopMovement();
		}
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_TestEscapeStep::ClearNeedToEscape(UBehaviorTreeComponent& OwnerComp) const
{
	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		if (!NeedToEscapeKey.SelectedKeyName.IsNone())
		{
			BB->SetValueAsBool(NeedToEscapeKey.SelectedKeyName, false);
		}
	}
}

bool UBTTask_TestEscapeStep::ProjectToNav(UWorld* World, const FVector& In, FVector& Out) const
{
	if (!World) return false;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys) return false;

	FNavLocation NavLoc;
	const FVector Extent(NavProjectExtentXY, NavProjectExtentXY, NavProjectExtentZ);

	if (NavSys->ProjectPointToNavigation(In, NavLoc, Extent))
	{
		Out = NavLoc.Location;
		return true;
	}
	return false;
}

FVector UBTTask_TestEscapeStep::ComputeEscapeGoal(AAIController* AIC, UBlackboardComponent* BB, bool bTryOpposite, bool& bOutFoundBaseDir) const
{
	bOutFoundBaseDir = false;

	APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
	if (!Pawn) return FVector::ZeroVector;

	const FVector MyLoc = Pawn->GetActorLocation();

	// 기준 방향(baseDir): 목표(Reserved/Base) 방향 or 컨트롤러 yaw or Pawn forward
	FVector BaseDir = Pawn->GetActorForwardVector();

	AActor* Reserved = nullptr;
	AActor* Base = nullptr;

	if (BB && !ReservedTargetKey.SelectedKeyName.IsNone())
	{
		Reserved = Cast<AActor>(BB->GetValueAsObject(ReservedTargetKey.SelectedKeyName));
	}
	if (BB && !EnemyBaseKey.SelectedKeyName.IsNone())
	{
		Base = Cast<AActor>(BB->GetValueAsObject(EnemyBaseKey.SelectedKeyName));
	}

	AActor* GoalActor = nullptr;
	if (bUseGoalBasedSideStep)
	{
		GoalActor = Reserved ? Reserved : Base;
	}

	if (GoalActor)
	{
		FVector ToGoal = GoalActor->GetActorLocation() - MyLoc;
		ToGoal.Z = 0.f;
		if (!ToGoal.IsNearlyZero())
		{
			BaseDir = ToGoal.GetSafeNormal();
			bOutFoundBaseDir = true;
		}
	}
	else if (bUseControllerYawFallback && AIC)
	{
		const FRotator YawRot(0.f, AIC->GetControlRotation().Yaw, 0.f);
		BaseDir = YawRot.Vector().GetSafeNormal();
	}

	// 좌/우 방향
	const FVector Right = FVector::CrossProduct(FVector::UpVector, BaseDir).GetSafeNormal();
	const bool bPickRight = (FMath::RandBool() ^ bTryOpposite); // 반대 재시도면 반전

	FVector Side = bPickRight ? Right : -Right;
	Side.Z = 0.f;
	Side = Side.GetSafeNormal();

	// 최종 목표
	return MyLoc + Side * StepDistance;
}
