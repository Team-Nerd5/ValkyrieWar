// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "GameSystem/State/Game/BattleGameState.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Module/StageModule.h"
#include "Data/Module/StageInfoModule.h"
#include "Data/Module/StageRewardModule.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"

void UBattleResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UILayer = EUILayer::POPUP;

	BindDelegates();
	RefreshResultUI();
}

void UBattleResultWidget::NativeDestruct()
{
	UnbindDelegates();

	Super::NativeDestruct();
}

void UBattleResultWidget::SetBattleResult(EBattleState InBattleState, TArray<int32> InRewards)
{
	if (!IsValidResultState(InBattleState))
	{
		return;
	}

	BattleState = InBattleState;
	if (BattleState == EBattleState::Win)
	{
		UpdateBattleReward(InRewards);
	}
	RefreshResultUI();
}

void UBattleResultWidget::UpdateBattleReward(TArray<int32> InRewards)
{
	UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;

	if (Widget_Win)
	{
		for (int32 DataId : InRewards)
		{
			FRewardDataRow Reward = DataManager->GetRewardModule()->FindRewardByDataId(DataId);
			if(Reward.DataId > 0)
				Widget_Win->SetReward(Reward);
		}
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
		if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
		{
			DataManager->GetStageInfoModule()->SetNextStage();
		}
		WorldEventSystem->Battle.OnBattleStateChanged.Broadcast(EBattleState::MoveToLobby);
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
