// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/GachaWidget.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/Game/UIManager.h"

void UGachaWidget::NativeConstruct()
{
	Super::NativeConstruct();

    CreateTopMenu();
    RefreshUI();

    if (Btn_Summon_1x)
    {
        Btn_Summon_1x->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon1x);
    }
    if (Btn_Summon_10x)
    {
        Btn_Summon_10x->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon10x);
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
}

void UGachaWidget::OnClickSummon1x()
{
    // TODO : 가챠연출? & 서버에 패킷전송
}

void UGachaWidget::OnClickSummon10x()
{
    // TODO : 위에 내용과 똑같음 * 10
}

void UGachaWidget::RefreshUI()
{
    Super::RefreshUI();

    int32 CurrentPity = 75;
    int32 MaxPity = 100;

    if (Text_PityCount)
    {
        FString PityStr = FString::Printf(TEXT("%d | %d"), CurrentPity, MaxPity);
        Text_PityCount->SetText(FText::FromString(PityStr));
    }
    if (PB_Pity)
    {
        float PityPercent = (float)CurrentPity / (float)MaxPity;
        PB_Pity->SetPercent(PityPercent);
    }
}
