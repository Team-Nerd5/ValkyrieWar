// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestAI/BTService_TestUpdateReservation.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "Test/SDCH/TestUnit/TestComponents/TestUnitBrainComponent.h"
#include "Test/SDCH/TestGameSystem/TestInstance/TestWorld/TestBattleDirectorSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_TestUpdateReservation::UBTService_TestUpdateReservation()
{
    NodeName = TEXT("Test Update Reservation");
    Interval = 0.25f;
    RandomDeviation = 0.05f;
}

void UBTService_TestUpdateReservation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return;

    ATestBaseUnit* Unit = Cast<ATestBaseUnit>(AIC->GetPawn());
    if (!Unit || !Unit->Brain) return;

    UWorld* World = Unit->GetWorld();
    if (!World) return;

    if (UTestBattleDirectorSubsystem* Subsys = World->GetSubsystem<UTestBattleDirectorSubsystem>())
    {
        Subsys->UpdateReservationFor(Unit);
    }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    BB->SetValueAsObject(ReservedTargetKey.SelectedKeyName, Unit->Brain->ReservedTarget.Get());
    BB->SetValueAsObject(EnemyBaseKey.SelectedKeyName, Unit->Brain->EnemyBase.Get());
}
