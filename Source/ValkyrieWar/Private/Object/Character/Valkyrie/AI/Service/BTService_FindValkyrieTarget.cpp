#include "Object/Character/Valkyrie/AI/Service/BTService_FindValkyrieTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Object/Character/Unit/UnitCharacter.h"

UBTService_FindValkyrieTarget::UBTService_FindValkyrieTarget()
{
	NodeName = TEXT("Find Valkyrie Target");

	Interval = 0.3f;
	RandomDeviation = 0.1f;

	// BTService_BlackboardBase의 기본 BlackboardKey = TargetActor 용
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindValkyrieTarget, BlackboardKey), AActor::StaticClass());

	// 추가 Blackboard Key = CombatRange 용
	CombatRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindValkyrieTarget, CombatRangeKey));
}

void UBTService_FindValkyrieTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	TickNode(OwnerComp, NodeMemory, 0.0f);
}

void UBTService_FindValkyrieTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	AValkyrieCharacter* Valkyrie = Cast<AValkyrieCharacter>(AIController->GetPawn());
	if (!Valkyrie)
	{
		return;
	}

	UWorld* World = Valkyrie->GetWorld();
	if (!World)
	{
		return;
	}

	UBattleDirectorSubsystem* BattleDirector = World->GetSubsystem<UBattleDirectorSubsystem>();
	if (!BattleDirector)
	{
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* NewTarget = BattleDirector->GetNearestAttackTargetForValkyrie(
		Valkyrie->GetActorLocation(),
		Valkyrie->GetTeamType(),
		EnemyDetectRadius
	);

	AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject(GetSelectedBlackboardKey()));
	AUnitCharacter* UnitTarget = Cast<AUnitCharacter>(CurrentTarget);

	if (UnitTarget && !UnitTarget->IsDead())
	{
		return;
	}

	if (CurrentTarget != NewTarget)
	{
		BB->SetValueAsObject(GetSelectedBlackboardKey(), NewTarget);

		if (NewTarget)
		{
			UE_LOG(LogTemp, Log, TEXT("[BTService_FindValkyrieTarget] Target Changed -> %s"), *NewTarget->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[BTService_FindValkyrieTarget] Target Cleared"));
		}
	}

	// CombatRange 갱신
	float NewCombatRange = 0.0f;
	if (NewTarget)
	{
		NewCombatRange = Valkyrie->GetAttackRange();
	}

	BB->SetValueAsFloat(CombatRangeKey.SelectedKeyName, NewCombatRange);
}
