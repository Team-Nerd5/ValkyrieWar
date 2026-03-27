// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/BattleTimerWidget.h"
#include "Components/TextBlock.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

void UBattleTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnInGameTimeChanged.AddDynamic(this, &UBattleTimerWidget::OnTimeChanged);
	}

	UpdateTimeTextAndColor(0.f);
}

void UBattleTimerWidget::NativeDestruct()
{
	StopBlink();

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnInGameTimeChanged.RemoveDynamic(this, &UBattleTimerWidget::OnTimeChanged);
	}

	Super::NativeDestruct();
}

void UBattleTimerWidget::OnTimeChanged(float InCurrentTime)
{
	UpdateTimeTextAndColor(InCurrentTime);

	const int32 TotalSec = FMath::Max(0, FMath::FloorToInt(InCurrentTime));

	if (TotalSec <= BlinkSeconds)
	{
		StartBlink();
	}
	else
	{
		StopBlink();
	}
}

void UBattleTimerWidget::UpdateTimeTextAndColor(float InSeconds)
{
	if (!TimeText) return;

	const int32 TotalSec = FMath::Max(0, FMath::FloorToInt(InSeconds));
	const int32 Min = TotalSec / 60;
	const int32 Sec = TotalSec % 60;

	TimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Min, Sec)));

	if (TotalSec <= WarningSeconds)
	{
		TimeText->SetColorAndOpacity(FSlateColor(WarningColor));
	}
	else
	{
		StopBlink();
		TimeText->SetColorAndOpacity(FSlateColor(NormalColor));
		TimeText->SetRenderOpacity(1.f);
	}
}

void UBattleTimerWidget::StartBlink()
{
	if (bBlinking) return;
	if (!GetWorld() || !TimeText) return;

	bBlinking = true;
	bBlinkOn = true;

	ToggleBlink();

	GetWorld()->GetTimerManager().SetTimer(
		BlinkTimerHandle,
		this,
		&UBattleTimerWidget::ToggleBlink,
		BlinkInterval,
		true
	);
}

void UBattleTimerWidget::StopBlink()
{
	if (!bBlinking) return;

	bBlinking = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	}

	if (TimeText)
	{
		TimeText->SetRenderOpacity(1.f);
	}
}

void UBattleTimerWidget::ToggleBlink()
{
	if (!TimeText) return;

	bBlinkOn = !bBlinkOn;

	const float Opacity = bBlinkOn ? 1.0f : 0.3f;
	TimeText->SetRenderOpacity(Opacity);
}
