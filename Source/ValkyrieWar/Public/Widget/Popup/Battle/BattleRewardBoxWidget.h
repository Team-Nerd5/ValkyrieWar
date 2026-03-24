// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Data/Enum/DataEnums.h"
#include "Data/Table/GameData/RewardDataRow.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

#include "BattleRewardBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleRewardBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRewardItem(FRewardDataRow InRewardDataRow);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> RewardIcon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RewardAmount = nullptr;

};
