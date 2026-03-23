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

	TestRewardWidget(EBattleState::Win);
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

void UBattleResultWidget::SetBattleReward(TMap<EGoodsType, int32> InRewardGoods, int32 InStageRewardGroupId)
{
	// TODO: 현재 StageGoodsReward 관련 모듈 및 데이터가 없어 추후 생긴다면 코드 수정 필요

	if (Widget_Win)
	{
		Widget_Win->SetReward(InRewardGoods, InStageRewardGroupId);
	}
}

void UBattleResultWidget::TestRewardWidget(EBattleState InBattleState)
{
	if (!IsValidResultState(InBattleState))
		return;

	// 테스트용
	BattleState = InBattleState;

	TMap<EGoodsType, int32> TestRewardGoods;
	TestRewardGoods.Add(EGoodsType::Gold, 30);
	TestRewardGoods.Add(EGoodsType::Gem, 0);
	TestRewardGoods.Add(EGoodsType::Ticket, 50);

	int32 StageRewardGroupId = 500001;

	if (Widget_Win)
	{
		Widget_Win->SetReward(TestRewardGoods, StageRewardGroupId);
	}
	RefreshResultUI();
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
	if(Widget_Win)
		Widget_Win->SetVisibility(BattleState == EBattleState::Win ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if(Widget_Defeat)
		Widget_Defeat->SetVisibility(BattleState == EBattleState::Defeat ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	if (GoToNextLevelBox)
		GoToNextLevelBox->SetVisibility(BattleState == EBattleState::Win ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	
}

bool UBattleResultWidget::IsValidResultState(EBattleState InState) const
{
	return InState == EBattleState::Win
		|| InState == EBattleState::Defeat
		|| InState == EBattleState::TimeOver;
}
