#include "Object/Character/Valkyrie/Controller/ValkyrieAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

AValkyrieAIController::AValkyrieAIController()
{
	PrimaryActorTick.bCanEverTick = false;

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
}

void AValkyrieAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		BattleDirector = World->GetSubsystem<UBattleDirectorSubsystem>();
	}
}

void AValkyrieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ValkyrieAI] Possessed Pawn: %s"), *InPawn->GetName());
	}

	if (!BattleDirector)
	{
		if (UWorld* World = GetWorld())
		{
			BattleDirector = World->GetSubsystem<UBattleDirectorSubsystem>();
		}
	}
	if (!BTAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ValkyrieAI] BTAsset is invalid."));
	}
	if (BTAsset && BTAsset->BlackboardAsset)
	{
		BlackboardComp->InitializeBlackboard(*BTAsset->BlackboardAsset);
		BehaviorTreeComp->StartTree(*BTAsset);

		UE_LOG(LogTemp, Warning, TEXT("[ValkyrieAI] BehaviorTree Started: %s"), *BTAsset->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ValkyrieAI] BTAsset or BlackboardAsset is invalid."));
	}
}

void AValkyrieAIController::OnUnPossess()
{
	Super::OnUnPossess();

	if (BlackboardComp)
	{
		BlackboardComp->ClearValue(TargetActorKeyName);
	}
}
