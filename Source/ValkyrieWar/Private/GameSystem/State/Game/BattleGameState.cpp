// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Game/BattleGameState.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/World/SpawnUpgradeSubsystem.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"

#include "Widget/HUD/BattleWidget.h"
#include "Widget/Loading/LoadingWidget.h"

void ABattleGameState::ChangeState(EBattleState InState)
{
	State = InState;

	switch (State)
	{
	case EBattleState::Init:
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			UBattleWidget* BattleUI = UIManager->OpenUI<UBattleWidget>(EUIType::Battle);

			if (!BattleUI)
			{
				UE_LOG(LogTemp, Error, TEXT("BattleUI is not exist!! Can't start game"));
				return;
			}

			UIManager->CloseUI<ULoadingWidget>(EUIType::Loading);

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				AValkyrieCharacterController* VPC = Cast<AValkyrieCharacterController>(PC);
				if (VPC)
				{
					VPC->SetBattleUI(BattleUI);
				}
			}
		}
		break;

	case EBattleState::Ready:
		break;

	case EBattleState::Start:
		break;

	case EBattleState::Play:
		PlayGame();
		break;

	case EBattleState::Win:
		//결과 UI를 Win으로 열어줌
		StopStageTimer();
		UE_LOG(LogTemp, Warning, TEXT("Win!"));
		break;

	case EBattleState::Defeat:
		//결과 UI를 Defeat으로 열여줌
		StopStageTimer();
		UE_LOG(LogTemp, Warning, TEXT("Lose.."));
		break;

	case EBattleState::TimeOver:
		// 결과 UI를 Draw로 열어줌
		StopStageTimer();
		UE_LOG(LogTemp, Warning, TEXT("TimeOver!"));
		break;

	case EBattleState::MoveToLobby:
		StopStageTimer();
		if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
		{
			//레벨 전환으로 가야할 듯..
		}
		break;

	default:
		break;
	}
}

void ABattleGameState::StartStageTimer(float InLimitSeconds)
{
	if (!GetWorld()) return;

	TimeLimitSeconds = FMath::Max(0.1f, InLimitSeconds);
	EndTimeSeconds = GetWorld()->GetTimeSeconds() + (double)TimeLimitSeconds;
	bTimerRunning = true;

	// 체크 주기는 UI 표시에 맞춰서 (초단위면 0.2~1.0)
	GetWorld()->GetTimerManager().ClearTimer(TimerCheckHandle);
	GetWorld()->GetTimerManager().SetTimer(
		TimerCheckHandle,
		this,
		&ABattleGameState::CheckTimeOver,
		0.2f,
		true
	);
}

void ABattleGameState::StopStageTimer()
{
	bTimerRunning = false;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerCheckHandle);
	}
}

float ABattleGameState::GetRemainingSeconds() const
{
	if (!bTimerRunning || !GetWorld()) return 0.f;

	const double Now = GetWorld()->GetTimeSeconds();
	return (float)FMath::Max(0.0, EndTimeSeconds - Now);
}

void ABattleGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopStageTimer();
	Super::EndPlay(EndPlayReason);
}

void ABattleGameState::PlayGame()
{
	// 플레이 시작 시점에 타이머 시작
	StartStageTimer(TimeLimitSeconds);

	if (USpawnUpgradeSubsystem* Sub = GetWorld()->GetSubsystem<USpawnUpgradeSubsystem>())
	{
		Sub->BindUpgradeDelegates();
	}
}

void ABattleGameState::CheckTimeOver()
{
	if (!bTimerRunning) return;

	float Remaining = GetRemainingSeconds();

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Battle.OnInGameTimeChanged.Broadcast(Remaining);

		// 테스트용
		// TODO: 발키리가 적 유닛 공격하여 사망 가능할 시 아래 코드 제거
		EventSystem->Battle.OnManaAdd.Broadcast(10);
	}

	if (Remaining <= 0.f)
	{
		StopStageTimer();
		ChangeState(EBattleState::TimeOver);
	}
}
