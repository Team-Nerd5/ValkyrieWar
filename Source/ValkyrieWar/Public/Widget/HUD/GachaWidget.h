// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Engine/DataTable.h"
#include "GachaWidget.generated.h"

USTRUCT(BlueprintType)
struct FTestItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UTexture2D* ItemIcon;
};

UCLASS()
class VALKYRIEWAR_API UGachaWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Btn_Summon_1x; // 1회 소환 

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Btn_Summon_10x; // 10회 소환 

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Btn_WhatIs_CeliGacha; // 천장 가챠 시스템 설명 

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> PB_GachaCount; // 천장 가챠 프로그레스 바 

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Text_GachaCount; // 가챠 횟수

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Text_CurrentTicket;// 현재 유저의 가챠 티켓수

protected: // 테스트 용 나중에 지우기 
    // 🧺 띄워줄 팝업창 WBP 클래스
    UPROPERTY(EditAnywhere, Category = "Gacha Test")
    TSubclassOf<class UGachaResultWidget> PopupWidgetClass;

    // 📖 테스트용 데이터 테이블
    UPROPERTY(EditAnywhere, Category = "Gacha Test")
    class UDataTable* TestDataTable;

protected:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void RefreshUI() override;

    UFUNCTION()
    void OnClickSummon1x();

    UFUNCTION()
    void OnClickSummon10x();

    UFUNCTION()
    void OnClickWhatIs_CeliGacha();

    void UpdateGachaData(int32 AddCount);

    void SimulateGacha(int32 PullCount);
};
