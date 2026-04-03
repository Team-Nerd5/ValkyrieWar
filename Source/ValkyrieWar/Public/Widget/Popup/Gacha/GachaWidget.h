// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Engine/DataTable.h"
#include "GachaWidget.generated.h"

UCLASS()
class VALKYRIEWAR_API UGachaWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Btn_Summon_1; // 1회 소환 

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Btn_Summon_10; // 10회 소환 

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> CeilProgressBar; // 천장 가챠 프로그레스 바 

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> CeilProgressText; // 가챠 횟수

    int32 SelectedGroupId = 0;

protected:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    virtual void OpenUI() override;

    UFUNCTION()
    void OnClickSummon1();

    UFUNCTION()
    void OnClickSummon10();
};
