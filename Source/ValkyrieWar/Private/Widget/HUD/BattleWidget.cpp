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
