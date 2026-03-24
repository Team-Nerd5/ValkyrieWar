// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ScrollBox.h"

#include "Widget/Popup/Battle/BattleRewardBoxWidget.h"

#include "Data/Table/GameData/RewardDataRow.h"

#include "BattleWinWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleWinWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetReward(TArray<FRewardDataRow> InRewardList);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> RewardListBox = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI|WidgetClass")
	TSubclassOf<UBattleRewardBoxWidget> RewardBoxWidgetClass = nullptr;
};
