// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/WallHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

void UWallHealthBarWidget::Init()
{
    BindDelegates();
    ApplyHealth(MaxHealth);
}

void UWallHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BindDelegates();
    ApplyHealth(MaxHealth);
}

void UWallHealthBarWidget::NativeDestruct()
{
    UnbindDelegates();
    Super::NativeDestruct();
}

void UWallHealthBarWidget::BindDelegates()
{
    if (bBound) return;

    if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
    {
        if (Team == ETeam::TeamA)
        {
            WorldEventSystem->Battle.OnAllyWallHealthChanged.AddDynamic(
                this, &UWallHealthBarWidget::HandleAllyWallHealthChanged);
        }
        else
        {
            WorldEventSystem->Battle.OnEnemyWallHealthChanged.AddDynamic(
                this, &UWallHealthBarWidget::HandleEnemyWallHealthChanged);
        }

        bBound = true;
    }
}

void UWallHealthBarWidget::UnbindDelegates()
{
    if (!bBound) return;

    if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
    {
        WorldEventSystem->Battle.OnAllyWallHealthChanged.RemoveDynamic(
            this, &UWallHealthBarWidget::HandleAllyWallHealthChanged);

        WorldEventSystem->Battle.OnEnemyWallHealthChanged.RemoveDynamic(
            this, &UWallHealthBarWidget::HandleEnemyWallHealthChanged);
    }

    bBound = false;
}

void UWallHealthBarWidget::HandleAllyWallHealthChanged(float InCur, float InMax)
{
    if (Team != ETeam::TeamA) return;
    MaxHealth = FMath::Max(1.f, InMax);
    ApplyHealth(InCur);
}

void UWallHealthBarWidget::HandleEnemyWallHealthChanged(float InCur, float InMax)
{
    if (Team != ETeam::TeamB) return;
    MaxHealth = FMath::Max(1.f, InMax);
    ApplyHealth(InCur);
}

void UWallHealthBarWidget::ApplyHealth(float InNewHealth)
{
    if (!HealthBar) return;

    const float SafeMax = FMath::Max(1.f, MaxHealth);
    const float Clamped = FMath::Clamp(InNewHealth, 0.f, SafeMax);

    const float Ratio = Clamped / SafeMax;
    const float Percent = Ratio * 100.f;

    HealthBar->SetPercent(Ratio);

    // 퍼센트 숫자 표시 (82)
    if (PercentText)
    {
        const int32 DisplayPercent = FMath::RoundToInt(Percent);
        PercentText->SetText(FText::AsNumber(DisplayPercent));
    }

    UE_LOG(LogTemp, Log, TEXT("Wall Damaged : %.2f (%.0f)"), InNewHealth, Percent);
}
