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
	virtual void NativeConstruct() override;
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

public:
	FORCEINLINE FVector2D const GetJoyPadAxis() { return JoyPadAxis; }

	void SetJoyPadVisibility(bool bIsVisible);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> JoyPadImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> JoyPadBGImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Frame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Arrows;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWallHealthBarWidget> AllyWallHealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWallHealthBarWidget> EnemyWallHealthBar;

private:
	EInputControlMode CurrentMode = EInputControlMode::Manual;

	bool bIsTouching = false;

	FVector2D JoyPadAxis = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	float JoyPadArea = 100.0f;
};
