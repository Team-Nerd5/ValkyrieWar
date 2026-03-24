// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "GameSystem/State/Game/BattleGameState.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Module/StageRewardModule.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"

void UBattleResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UILayer = EUILayer::POPUP;

	BindDelegates();
	RefreshResultUI();

	// 테스트용
	SetBattleResult(EBattleState::Win);
	SetBattleReward(100101);
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

void UBattleResultWidget::SetBattleReward(int32 InStageRewardGroupId)
{
	UDataManager* Datamanager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!Datamanager)
		return;
	UStageRewardModule* StageRewardModule = Datamanager->GetStageRewardModule();
	if (!StageRewardModule)
		return;

	InRewardList.Empty();
	if (Widget_Win)
	{
		StageRewardModule->GetRewardRowsByStageRewardGroupId(InStageRewardGroupId, InRewardList);
		Widget_Win->SetReward(InRewardList);
	}
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

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		// TODO: 다음 레벨로 이동 -> 레벨 변경 구현 필요
		WorldEventSystem->Battle.OnBattleStateChanged.Broadcast(EBattleState::MoveToNextLevel);
	}
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
	if(Widget_Win)
		Widget_Win->SetVisibility(BattleState == EBattleState::Win ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (Widget_Defeat)
		Widget_Defeat->SetVisibility(
			BattleState == EBattleState::Defeat || BattleState == EBattleState::TimeOver ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	if (GoToNextLevelBox)
		GoToNextLevelBox->SetVisibility(BattleState == EBattleState::Win ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);	
}

bool UBattleResultWidget::IsValidResultState(EBattleState InState) const
{
	return InState == EBattleState::Win
		|| InState == EBattleState::Defeat
		|| InState == EBattleState::TimeOver;
}
