// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Gacha/GachaWidget.h"
#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Engine/DataTable.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/DataManager.h"


void UGachaWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (Btn_Summon_1)
    {
        Btn_Summon_1->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon1);
    }
    if (Btn_Summon_10)
    {
        Btn_Summon_10->OnClicked.AddDynamic(this, &UGachaWidget::OnClickSummon10);
    }
}

void UGachaWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (Btn_Summon_1)
    {
        Btn_Summon_1->OnClicked.RemoveDynamic(this, &UGachaWidget::OnClickSummon1);
    }
    if (Btn_Summon_10)
    {
        Btn_Summon_10->OnClicked.RemoveDynamic(this, &UGachaWidget::OnClickSummon10);
    }
}

void UGachaWidget::OpenUI()
{
    UIType = EUIType::PopupGacha;

    Super::OpenUI();

    //임시 처리(원래 소환 열어주면서 세팅.. 가챠 테이블 데이터가 있어야 하는데...)
    SelectedGroupId = 70001;

    int64 GachaPrice = 100;
    //가챠 가격 정보를 안만들어서... 우선 숫자 입력
    if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
    {
        if (Btn_Summon_1)
            Btn_Summon_1->SetIsEnabled(DataManager->GetGoodsModule()->IsEnough(EGoodsType::Gem, GachaPrice));

        if (Btn_Summon_10)
            Btn_Summon_10->SetIsEnabled(DataManager->GetGoodsModule()->IsEnough(EGoodsType::Gem, GachaPrice * 10));
    }

    if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
    {
        uint64 current = SaveManager->GetGachaAmount();
        int32 ceilValue = SaveManager->GetCeilValue();

        if (CeilProgressText)
            CeilProgressText->SetText(FText::Format(FText::FromString(TEXT("{0}/{1}")), current, ceilValue));

        if (CeilProgressBar)
            CeilProgressBar->SetPercent(static_cast<float>(current) / ceilValue);
    }
}

void UGachaWidget::OnClickSummon1()
{
    if (EventSystem)
    {
        EventSystem->Lobby.OnLoadGacha.Broadcast(1, SelectedGroupId);
    }
    if(Btn_Summon_1)
        Btn_Summon_1->SetIsEnabled(false);
    if (Btn_Summon_10)
        Btn_Summon_10->SetIsEnabled(false);
}

void UGachaWidget::OnClickSummon10()
{
    if (EventSystem)
    {
        EventSystem->Lobby.OnLoadGacha.Broadcast(10, SelectedGroupId);
    }
    if (Btn_Summon_1)
        Btn_Summon_1->SetIsEnabled(false);
    if (Btn_Summon_10)
        Btn_Summon_10->SetIsEnabled(false);
}
