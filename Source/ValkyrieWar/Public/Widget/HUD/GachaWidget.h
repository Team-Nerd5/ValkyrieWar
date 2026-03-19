// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "GachaWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGachaWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Btn_Summon_1x;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Btn_Summon_10x;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> PB_Pity;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Text_PityCount;
protected:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void RefreshUI() override;

    UFUNCTION()
    void OnClickSummon1x();

    UFUNCTION()
    void OnClickSummon10x();
};
