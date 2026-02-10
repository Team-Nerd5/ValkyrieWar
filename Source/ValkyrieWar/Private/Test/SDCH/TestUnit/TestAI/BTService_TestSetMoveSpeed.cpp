// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestAI/BTService_TestSetMoveSpeed.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

void UBTService_TestSetMoveSpeed::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	ACharacter* Char = AIC ? Cast<ACharacter>(AIC->GetPawn()) : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!Char || !BB) return;

	const UObject* TargetObj = BB->GetValueAsObject(TargetKey.SelectedKeyName);
	const bool bHasTarget = (TargetObj != nullptr);

	if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
	{
		Move->MaxWalkSpeed = bHasTarget ? RunSpeed : WalkSpeed;
	}
}
