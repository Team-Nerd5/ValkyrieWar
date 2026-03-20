// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleResultWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameSystem/State/Game/BattleGameState.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

void UBattleResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UILayer = EUILayer::POPUP;

	BindDelegates();
	RefreshResultUI();

	SetBattleResult(EBattleState::Defeat);
}

void UBattleResultWidget::NativeDestruct()
{
	UnbindDelegates();

	Super::NativeDestruct();
}

void UBattleResultWidget::SetBattleResult(EBattleState InBattleState)
{
	if (!IsValidResultState(InBattleState))
	{
		return;
	}

	BattleState = InBattleState;
	RefreshResultUI();
}

void UBattleResultWidget::SetBattleReward(TArray<int32> InRewardList)
{
	// TODO: 받을 보상 세팅(필요시 파라메터 추가)

	// RewardDataRow의 DataId를 받고 표시

	// 추후 플레이어 재화 업데이트 필요
}

void UBattleResultWidget::HandleBackToLobbyClicked()
{
	OnCloseUIRequested.Broadcast(this);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleStateChanged.Broadcast(EBattleState::MoveToLobby);
	}
}

void UBattleResultWidget::HandleGoToNextLevelClicked()
{
	OnCloseUIRequested.Broadcast(this);

	// TODO: 다음 레벨로 이동
}

void UBattleResultWidget::BindDelegates()
{
	if (BackToLobbyButton)
	{
		BackToLobbyButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleBackToLobbyClicked);
	}
	if (GoToNextLevelButton)
	{
		GoToNextLevelButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleGoToNextLevelClicked);
	}
}

void UBattleResultWidget::UnbindDelegates()
{
	if (BackToLobbyButton)
	{
		BackToLobbyButton->OnClicked.RemoveDynamic(this, &UBattleResultWidget::HandleBackToLobbyClicked);
	}
	if (GoToNextLevelButton)
	{
		GoToNextLevelButton->OnClicked.RemoveDynamic(this, &UBattleResultWidget::HandleGoToNextLevelClicked);
	}
}

void UBattleResultWidget::RefreshResultUI()
{
	if(VictoryBox)
		VictoryBox->SetVisibility(BattleState == EBattleState::Win ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	if(LoseBox)
		LoseBox->SetVisibility(BattleState == EBattleState::Defeat ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	// 패배 했을 때도 보상이 있다면
	// -> 보상이 존재 할 때만 보상 목록 표시

	if (GoToNextLevelBox)
		GoToNextLevelButton->SetVisibility(BattleState == EBattleState::Win ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	
}

bool UBattleResultWidget::IsValidResultState(EBattleState InState) const
{
	return InState == EBattleState::Win
		|| InState == EBattleState::Defeat
		|| InState == EBattleState::TimeOver;
}
