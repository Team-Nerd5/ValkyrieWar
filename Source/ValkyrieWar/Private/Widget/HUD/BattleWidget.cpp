// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/BattleWidget.h"
#include "Components/Image.h"

FReply UBattleWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bIsTouching = true;
	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UBattleWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (bIsTouching)
	{
		if (JoyPadBGImage)
		{
			 FVector2D BGLocation = JoyPadBGImage->GetCachedGeometry().AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());
			 FVector JoyPadSize = FVector(BGLocation, 0.0f);

			 JoyPadSize = JoyPadSize.GetClampedToMaxSize2D(JoyPadArea);
			 
			 JoyPadImage->SetRenderTranslation(FVector2D(JoyPadSize));

			 JoyPadAxis = FVector2D(JoyPadSize / JoyPadArea);

			FReply::Handled();
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
