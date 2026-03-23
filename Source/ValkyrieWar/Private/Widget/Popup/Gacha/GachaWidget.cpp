// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Gacha/GachaWidget.h"
#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Engine/DataTable.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"


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
        Btn_WhatIs_CeliGacha->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon10x);
    }
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
        Btn_WhatIs_CeliGacha->OnClicked.RemoveDynamic(this, &UGachaWidget::OnClickSummon10x);
    }
}

void UGachaWidget::OpenUI()
{
    UIType = EUIType::PopupGacha;

    Super::OpenUI();

    //임시 처리(원래 소환 열어주면서 세팅.. 가챠 테이블 데이터가 있어야 하는데...)
    SelectedGroupId = 70001;
}

void UGachaWidget::OnClickSummon1x()
{
    if (EventSystem)
    {
        EventSystem->Lobby.OnLoadGacha.Broadcast(1, SelectedGroupId);
    }
}

void UGachaWidget::OnClickSummon10x()
{
    if (EventSystem)
    {
        EventSystem->Lobby.OnLoadGacha.Broadcast(10, SelectedGroupId);
    }
}

void UGachaWidget::OnClickWhatIs_CeliGacha()
{
    UE_LOG(LogTemp, Warning, TEXT("천장 가챠 설명 버튼 바인딩"));
}

void UGachaWidget::UpdateGachaData(int32 AddCount)
{
    //??
}
