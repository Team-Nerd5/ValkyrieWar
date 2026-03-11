// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/DataEnums.h"
#include "GoodsItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGoodsItemWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Icon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AmountText = nullptr;

	EGoodsType CurrentType = EGoodsType::None;
public:

	UFUNCTION()
	void OnGoodsChange(EGoodsType InGoodsType, uint64 InAmount);
	void SetData(EGoodsType InGoodsType);
};
