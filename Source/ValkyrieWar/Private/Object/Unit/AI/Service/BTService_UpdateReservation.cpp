// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Unit/AI/Service/BTService_UpdateReservation.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Unit/Component/UnitBrainComponent.h"
#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_UpdateReservation::UBTService_UpdateReservation()
{
    NodeName = TEXT("Update Reservation");
    Interval = 0.25f;
    RandomDeviation = 0.05f;
}

void UBTService_UpdateReservation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return;

    AUnitCharacter* Unit = Cast<AUnitCharacter>(AIC->GetPawn());
    if (!Unit || !Unit->GetBrain()) return;

    UWorld* World = Unit->GetWorld();
    if (!World) return;

    if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
    {
        Subsys->UpdateReservationFor(Unit);
    }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    BB->SetValueAsObject(ReservedTargetKey.SelectedKeyName, Unit->GetBrain()->ReservedTarget.Get());
    BB->SetValueAsObject(EnemyBaseKey.SelectedKeyName, Unit->GetBrain()->EnemyBase.Get());
}
