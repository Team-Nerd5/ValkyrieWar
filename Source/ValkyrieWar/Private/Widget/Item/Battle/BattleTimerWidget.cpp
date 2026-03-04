// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/BattleTimerWidget.h"
#include "Components/TextBlock.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

void UBattleTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 이벤트 바인딩 (너가 말한 방식)
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnInGameTimeChanged.AddDynamic(this, &UBattleTimerWidget::OnTimeChanged);
	}

	// 초기 상태
	UpdateTimeTextAndColor(0.f);
}

void UBattleTimerWidget::NativeDestruct()
{
	StopBlink();

	// 이벤트 언바인드 (크래시 방지)
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

	// 10초 이하: blink ON
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

	// 60초 이하: 빨강 / 그 외: 흰색
	if (TotalSec <= WarningSeconds)
	{
		TimeText->SetColorAndOpacity(FSlateColor(WarningColor));
		// 알파(깜빡임)는 ToggleBlink()에서 SetRenderOpacity로만 처리
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

	// 복구: 빨강 상태 유지(<=60초일 때), 알파는 1로
	if (TimeText)
	{
		TimeText->SetRenderOpacity(1.f);
	}
}

void UBattleTimerWidget::ToggleBlink()
{
	if (!TimeText) return;

	bBlinkOn = !bBlinkOn;

	// Opacity만 토글 (색은 빨강 유지)
	const float Opacity = bBlinkOn ? 1.0f : 0.3f;
	TimeText->SetRenderOpacity(Opacity);
}
