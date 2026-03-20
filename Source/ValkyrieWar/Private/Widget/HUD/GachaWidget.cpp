// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/GachaWidget.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"

#include "Object/SaveGame/GachaSaveGame.h"

void UGachaWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (Btn_Summon_1x)
    {
        Btn_Summon_1x->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon1x);
    }
    if (Btn_Summon_10x)
    {
        Btn_Summon_10x->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon10x);
    }
    if (Btn_WhatIs_CeliGacha)
    {
        Btn_Summon_10x->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon10x);
    }

    RefreshUI();
}

void UGachaWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (Btn_Summon_1x)
    {
        Btn_Summon_1x->OnClicked.RemoveDynamic(this, &UGachaWidget::OnClickSummon1x);
    }
    if (Btn_Summon_10x)
    {
        Btn_Summon_10x->OnClicked.RemoveDynamic(this, &UGachaWidget::OnClickSummon10x);
    }
    if (Btn_WhatIs_CeliGacha)
    {
        Btn_Summon_10x->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon10x);
    }
    RefreshUI();
}

void UGachaWidget::OnClickSummon1x()
{
    UE_LOG(LogTemp, Warning, TEXT("1회 가챠 버튼 바인딩"));
}

void UGachaWidget::OnClickSummon10x()
{
    UE_LOG(LogTemp, Warning, TEXT("10회 가챠 버튼 바인딩"));
}

void UGachaWidget::OnClickWhatIs_CeliGacha()
{
    UE_LOG(LogTemp, Warning, TEXT("천장 가챠 설명 버튼 바인딩"));
}

void UGachaWidget::UpdateGachaData(int32 AddCount)
{
}

void UGachaWidget::RefreshUI()
{
    Super::RefreshUI();
}
