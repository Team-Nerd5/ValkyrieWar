// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/BattleWidget.h"
#include "Components/Image.h"
#include "Widget/Item/Battle/WallHealthBarWidget.h"

void UBattleWidget::NativeConstruct()
{
	if (AllyWallHealthBar && EnemyWallHealthBar)
	{
		AllyWallHealthBar->Init();
		EnemyWallHealthBar->Init();
	}
}

FReply UBattleWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	// 자동모드라서 조이스틱 숨긴상황에선 조이스틱에 토치 안먹임
	if (JoyPadBGImage->GetVisibility() == ESlateVisibility::Hidden || JoyPadBGImage->GetVisibility() == ESlateVisibility::Collapsed)
	{
		return FReply::Unhandled();
	}

	// 조이스틱 안을 터치했나
	FVector2D LocalTouchPos = JoyPadBGImage->GetCachedGeometry().AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());
	FVector2D BGSize = JoyPadBGImage->GetCachedGeometry().GetLocalSize();

	if (LocalTouchPos.X >= 0.0f && LocalTouchPos.X <= BGSize.X && LocalTouchPos.Y >= 0.0f && LocalTouchPos.Y <= BGSize.Y)
	{
		bIsTouching = true;
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return FReply::Unhandled();
}

FReply UBattleWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (bIsTouching)
	{
		if (JoyPadBGImage)
		{
			 FVector2D BGLocation = JoyPadBGImage->GetCachedGeometry().AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());

			 //배경 이미지 정중앙 좌표 구하기
			 FVector2D BGCenter = JoyPadBGImage->GetCachedGeometry().GetLocalSize() / 2.0f;
			 // 터치 위치에서 중앙 값을 배제해서 정중앙 을 정돈함
			 FVector2D TouchFromCenter = BGLocation - BGCenter;

			 if (TouchFromCenter.Size() > JoyPadArea)
			 {
				 TouchFromCenter = TouchFromCenter.GetSafeNormal() * JoyPadArea;
			 }
			 
			 JoyPadImage->SetRenderTranslation(TouchFromCenter);

			 JoyPadAxis = TouchFromCenter / JoyPadArea;

			 return FReply::Handled();
		}
	}
	return FReply::Unhandled();
}

FReply UBattleWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bIsTouching = false;

	//조이패드 이미지 위치 리셋
	if (JoyPadImage)
		JoyPadImage->SetRenderTranslation(FVector2D::ZeroVector);

	//Axis 리셋
	JoyPadAxis = FVector2D::ZeroVector;

	return FReply::Handled().ReleaseMouseCapture();
}

void UBattleWidget::SetJoyPadVisibility(bool bIsVisible)
{
	ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	if (JoyPadImage) JoyPadImage->SetVisibility(NewVisibility);
	if (JoyPadBGImage) JoyPadBGImage->SetVisibility(NewVisibility);
	if (Frame) Frame->SetVisibility(NewVisibility);
	if (Arrows) Arrows->SetVisibility(NewVisibility);

	if (!bIsVisible)
	{
		JoyPadAxis = FVector2D::ZeroVector;
		if (JoyPadImage) JoyPadImage->SetRenderTranslation(FVector2D::ZeroVector);
		bIsTouching = false;
	}
}
