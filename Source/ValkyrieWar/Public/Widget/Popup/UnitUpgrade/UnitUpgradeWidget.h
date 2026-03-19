// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"

#include "Components/HorizontalBox.h"
#include "Widget/Popup/UnitUpgrade/UnitUpgradeBoxWidget.h"

#include "UnitUpgradeWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UUnitUpgradeWidget : public UBaseWidget
{
	GENERATED_BODY()
public:
	virtual void OpenUI() override;
protected:
	virtual void NativeConstruct() override;
protected:
	void InitUpgradeBox();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> UnitListBox = nullptr;

	// UnitListBox에 표시할 Box위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|WidgetClass")
	TSubclassOf<UUnitUpgradeBoxWidget> UpgradeWidgetClass = nullptr;

private:
	// 표시할 위젯들 저장
	TArray<TObjectPtr<UUnitUpgradeBoxWidget>> UpgradeBoxes;
};
