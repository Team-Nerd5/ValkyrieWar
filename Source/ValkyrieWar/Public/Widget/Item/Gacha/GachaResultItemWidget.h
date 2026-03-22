// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "GachaResultItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGachaResultItemWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Icon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Count;

public:
	void SetItemData(class UTexture2D* IconTexture, int32 ItemCount);
};
