// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/LoginWidget.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/LevelManager.h"
#include "GameSystem/Instance/Game/GameManager.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

FReply ULoginWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsReadyToStart)
    {
        OnStartGame();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply ULoginWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent)
{
    if (InTouchEvent.GetPointerIndex() == 0 && bIsReadyToStart)
    {
        OnStartGame();
        return FReply::Handled();
    }

    return Super::NativeOnTouchStarted(InGeometry, InTouchEvent);
}

void ULoginWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (EventSystem)
    {
        EventSystem->Login.OnReadyToStart.AddDynamic(this, &ULoginWidget::OnReadyToStart);
    }
}

void ULoginWidget::OnStartGame()
{
    if (EventSystem)
    {
        EventSystem->Login.OnLoginStateChanged.Broadcast(ELoginState::CheckNickname);
    }
}

void ULoginWidget::OnReadyToStart()
{
    if (StartText)
    {
        StartText->SetVisibility(ESlateVisibility::Visible);
    }
    bIsReadyToStart = true;
}

void ULoginWidget::OpenUI()
{
    Super::OpenUI();

    if (StartText)
    {
        StartText->SetVisibility(ESlateVisibility::Collapsed);
    }
}
