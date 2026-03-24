// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Data/Enum/DataEnums.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

#include "BattleRewardItemWIdget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleRewardItemWIdget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRewardItem(int32 InItemDataId, int32 InAmount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> RewardAmountBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RewardAmount = nullptr;
};
