// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/GachaWidget.h"
#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Engine/DataTable.h"


#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Base/BaseWidget.h"

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
    SimulateGacha(1);// 디버구용
}

void UGachaWidget::OnClickSummon10x()
{
    UE_LOG(LogTemp, Warning, TEXT("10회 가챠 버튼 바인딩"));
    SimulateGacha(10);// 디버구용
}

void UGachaWidget::OnClickWhatIs_CeliGacha()
{
    UE_LOG(LogTemp, Warning, TEXT("천장 가챠 설명 버튼 바인딩"));
}

void UGachaWidget::UpdateGachaData(int32 AddCount)
{
}

void UGachaWidget::SimulateGacha(int32 PullCount) //디버그용
{
    if (!PopupWidgetClass || !TestDataTable) return;

    TArray<FName> RowNames = TestDataTable->GetRowNames();
    if (RowNames.Num() == 0) return;

    // 테스트용 배열 준비!
    TArray<UTexture2D*> PulledIcons;
    TArray<int32> PulledCounts;

    for (int32 i = 0; i < PullCount; ++i)
    {
        int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
        // 🚨 구조체 이름(FTestItemData) 형네 프로젝트에 맞게 꼭 바꿔줘!!
        FTestItemData* RandomItem = TestDataTable->FindRow<FTestItemData>(RowNames[RandomIndex], TEXT("GachaTest"));

        if (RandomItem && RandomItem->ItemIcon)
        {
            PulledIcons.Add(RandomItem->ItemIcon);
            PulledCounts.Add(1);
        }
    }

    // 🧺 팝업창(주방장) 생성!!!
    UGachaResultWidget* ResultPopup = CreateWidget<UGachaResultWidget>(GetWorld(), PopupWidgetClass);
    if (ResultPopup)
    {
        // 1. 화면에 먼저 붙이고!
        ResultPopup->AddToViewport(999);

        // 🚨 2. 팀장님의 쌉근본 함수 호출!!! (여기서 Visible 켜지고 상단 메뉴 붙음!)
        ResultPopup->OpenUI();

        // 3. 주방장한테 요리 시작하라고 데이터 던져주기!!!
        // ResultPopup->ShowGachaResults(PulledIcons, PulledCounts); 
    }
}

void UGachaWidget::RefreshUI()
{
    Super::RefreshUI();
}
