// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "TestBattleModeToggleWidget.generated.h"

class UButton;
class UImage;

UCLASS()
class VALKYRIEWAR_API UTestBattleModeToggleWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ToggleButton;

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UImage> IconImage;

	UFUNCTION()
	void OnClickToggle();

	//void RefreshVisual();
};
