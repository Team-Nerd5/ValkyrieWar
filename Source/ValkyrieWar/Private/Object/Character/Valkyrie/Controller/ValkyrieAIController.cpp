#include "Object/Character/Valkyrie/Controller/ValkyrieAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AValkyrieAIController::AValkyrieAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (SightConfig)
	{
		SightConfig->SightRadius = AISightRadius;
		SightConfig->LoseSightRadius = AILoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = AISightAngle;

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		AIPerceptionComp->ConfigureSense(*SightConfig);
		AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AValkyrieAIController::OnTargetDetected);
	}
}

void AValkyrieAIController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("🟢 [AI_LOG] 뇌 생성 완료! (BeginPlay 실행됨)"));
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AValkyrieAIController::OnTargetDetected);
	}
}

void AValkyrieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("🔵 [AI_LOG] 빙의 완료! 조종할 몸뚱아리: %s"), *InPawn->GetName());
	}
	if (BTAsset && BTAsset->BlackboardAsset)
	{
		BlackboardComp->InitializeBlackboard(*(BTAsset->BlackboardAsset));
		BehaviorTreeComp->StartTree(*BTAsset);
	}
}

void AValkyrieAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (Actor == GetPawn())
	{
		return;
	}
	if (Stimulus.WasSuccessfullySensed())
	{
		if (Actor)
		{
			// 🟡 로그 3: 일단 시야에 뭔가 들어오면 무조건 찍음!
			UE_LOG(LogTemp, Warning, TEXT("🟡 [AI_LOG] 레이더에 뭔가 포착됨!: %s"), *Actor->GetName());
			FString TagList = TEXT("");
			for (FName TagName : Actor->Tags)
			{
				TagList += TagName.ToString() + TEXT(", ");
			}
			UE_LOG(LogTemp, Error, TEXT("🔍 엔진이 인식한 [%s]의 액터 태그 목록: [ %s ]"), *Actor->GetName(), *TagList);
			// 태그 검사 통과했는지 확인
			if (Actor->ActorHasTag(FName("Enemy")))
			{
				// 🔴 로그 4: 찐 타겟 발견! (빨간색 Error로 찍어서 눈에 확 띄게 만듦 ㅋㅋㅋ)
				UE_LOG(LogTemp, Error, TEXT("🔴 [AI_LOG] 타겟 확인! (Enemy 태그 일치) 이름: %s"), *Actor->GetName());

				if (BlackboardComp)
				{
					BlackboardComp->SetValueAsObject(FName("TargetEnemy"), Actor);
				}
			}
			else
			{
				// ⚪ 로그 5: 봤는데 적이 아닐 때
				UE_LOG(LogTemp, Warning, TEXT("⚪ [AI_LOG] 짭 타겟임. (Enemy 태그 없음) 무시."));
			}
		}
	}
}
