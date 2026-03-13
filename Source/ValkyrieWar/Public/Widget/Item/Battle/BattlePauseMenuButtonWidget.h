// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattlePauseMenuButtonWidget.generated.h"

/**
 * 
 */

class UButton;

UCLASS()
class VALKYRIEWAR_API UBattlePauseMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleMenuClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Menu = nullptr;
};
