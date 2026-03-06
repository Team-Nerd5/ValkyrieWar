// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Battle/BattleResultWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameSystem/State/Game/BattleGameState.h"

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

void UBattleResultWidget::SetBattleResult(EBattleState InBattleState)
{
	if (!IsValidResultState(InBattleState))
	{
		return;
	}

	BattleState = InBattleState;
	RefreshResultUI();
}

void UBattleResultWidget::HandleBackToLobbyClicked()
{
	OnCloseUIRequested.Broadcast(this);

	ABattleGameState* State = GetWorld()->GetGameState<ABattleGameState>();
	if (State)
	{
		State->ChangeState(EBattleState::MoveToLobby);
	}
}

void UBattleResultWidget::BindDelegates()
{
	if (BackToLobbyButton)
	{
		BackToLobbyButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleBackToLobbyClicked);
	}
}

void UBattleResultWidget::UnbindDelegates()
{
	if (BackToLobbyButton)
	{
		BackToLobbyButton->OnClicked.RemoveDynamic(this, &UBattleResultWidget::HandleBackToLobbyClicked);
	}
}

void UBattleResultWidget::RefreshResultUI()
{
	if (ResultText)
	{
		ResultText->SetText(GetHeaderText());
	}
}

FText UBattleResultWidget::GetHeaderText() const
{
	switch (BattleState)
	{
	case EBattleState::Win:
		return FText::FromString(TEXT("Win"));

	case EBattleState::Defeat:
		return FText::FromString(TEXT("Defeat"));

	case EBattleState::TimeOver:
		return FText::FromString(TEXT("Draw"));

	default:
		return FText::GetEmpty();
	}
}

bool UBattleResultWidget::IsValidResultState(EBattleState InState) const
{
	return InState == EBattleState::Win
		|| InState == EBattleState::Defeat
		|| InState == EBattleState::TimeOver;
}
