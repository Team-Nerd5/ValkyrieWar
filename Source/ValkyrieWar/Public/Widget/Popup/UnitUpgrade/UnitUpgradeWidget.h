// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"

#include "Widget/Popup/UnitUpgrade/UnitUpgradeBoxWidget.h"

#include "UnitUpgradeWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UUnitUpgradeWidget : public UBaseWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

protected:
	void InitUpgradeBox();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUnitUpgradeBoxWidget> UpgradeBox1 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUnitUpgradeBoxWidget> UpgradeBox2 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUnitUpgradeBoxWidget> UpgradeBox3 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUnitUpgradeBoxWidget> UpgradeBox4 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUnitUpgradeBoxWidget> UpgradeBox5 = nullptr;

};
