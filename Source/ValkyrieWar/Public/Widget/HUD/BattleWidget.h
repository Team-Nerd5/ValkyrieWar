// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "BattleWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleWidget : public UBaseWidget
{
	GENERATED_BODY()
public:
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

public:
	FORCEINLINE FVector2D const GetJoyPadAxis() { return JoyPadAxis; }
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> JoyPadImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> JoyPadBGImage;

private:
	bool bIsTouching = false;

	FVector2D JoyPadAxis = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	float JoyPadArea = 100.0f;
};
