// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/AI/Task/BTTask_RotateToTargetActor.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RotateToTargetActor::UBTTask_RotateToTargetActor()
{
	NodeName = TEXT("Rotate To Target Actor");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_RotateToTargetActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToTargetActor::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 방향 계산
	FVector Direction = TargetActor->GetActorLocation() - Pawn->GetActorLocation();
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	FRotator TargetRot = Direction.Rotation();
	FRotator CurrentRot = Pawn->GetActorRotation();

	// 부드러운 회전
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, RotationSpeed);
	Pawn->SetActorRotation(NewRot);

	// 각도 차이 체크
	float DeltaYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, TargetRot.Yaw));

	if (DeltaYaw <= AcceptableAngle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
