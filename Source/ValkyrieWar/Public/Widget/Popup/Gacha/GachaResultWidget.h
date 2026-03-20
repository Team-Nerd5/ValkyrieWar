// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "GachaResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGachaResultWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* WrapBox_Result;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gacha")
	TSubclassOf<class UGachaResultItemWidget> ItemWidgetClass;

public:
	UFUNCTION(BlueprintCallable)
	void ShowGachaResults(const TArray<int32>& ItemIDs, const TArray<int32>& ItemCounts);
	
};
