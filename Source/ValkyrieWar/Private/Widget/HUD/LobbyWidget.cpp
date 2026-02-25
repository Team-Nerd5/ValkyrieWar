// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/LobbyWidget.h"
#include "Data/Module/ContentsModule.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/LevelManager.h"

#include "GameSystem/Library/GameDataHelper.h"

#include "Widget/HUD/TopMenuWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

//TODO: 테스트 인벤토리.. 수정필요
#include "Test/OKM/TestInventoryWidget.h"

void ULobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StageButton)
    {
        StageButton->OnClicked.AddDynamic(this, &ULobbyWidget::ShowStageInternal);
    }
}

void ULobbyWidget::OpenUI()
{
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		//1. 탑메뉴 생성해서 세팅?
        CreateTopMenu();
		//
		//2. 컨텐츠 데이터 사용해서 버튼 세팅
		if (UContentsModule* Contents = DataManager->GetContentsModule())
		{
			TArray<FContentsDataRow*> ContentsList = Contents->GetAllDataSorted();

            int32 MaxColumns = 3;

			//버튼 생성
			for (FContentsDataRow* Data : ContentsList)
			{
                ULobbyMenuWidget* NewMenu = CreateWidget<ULobbyMenuWidget>(this, MenuItemClass);
                if (NewMenu)
                {
                    NewMenu->SetData(Data);
                    NewMenu->OnMenuButtonClicked.AddDynamic(this, &ULobbyWidget::OnClickInventory);

                    UUniformGridSlot* GridSlot = MenuPanel->AddChildToUniformGrid(NewMenu);

                    if (GridSlot)
                    {
                        int32 Row = Data->Order / MaxColumns;
                        int32 Col = Data->Order % MaxColumns;

                        GridSlot->SetRow(Row);
                        GridSlot->SetColumn(Col);

                        //꽉차게 정렬
                        GridSlot->SetHorizontalAlignment(HAlign_Fill);
                        GridSlot->SetVerticalAlignment(VAlign_Fill);
                    }
                }
			}
		}
	}

	Super::OpenUI();
}

void ULobbyWidget::CreateTopMenu()
{
    if (TopMenuClass)
    {
        FGoodsDataRow Data;

        if (UGameDataHelper::GetGoodsData(EUIType::Lobby, GetGameInstance(), Data))
        {
            UTopMenuWidget* TopMenu = CreateWidget<UTopMenuWidget>(this, TopMenuClass);

            if (TopMenu)
            {
                if (LobbyCanvas)
                {
                    UCanvasPanelSlot* CanvasSlot = LobbyCanvas->AddChildToCanvas(TopMenu);

                    CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.0f));
                    CanvasSlot->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 80.0f));
                }
                TopMenu->SetData(&Data);                    
            }
        }
    }
}

void ULobbyWidget::ShowStage(int32 InChapter, int32 InStageNum)
{
}

void ULobbyWidget::ShowInventory()
{
    if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
    {
        //이거 음..클래스 가져오는걸 음...
        UTestInventoryWidget* Widget = UIManager->OpenUI<UTestInventoryWidget>(EUIType::PopupInventory);

        //위젯 초기화
    }
}

void ULobbyWidget::ShowStageInternal()
{
    //TODO: 전투로 연결한거 바꿔야함..
    //우선은 그냥 전투 레벨로 전환

    if (ULevelManager* LevelManager = GetGameInstance()->GetSubsystem<ULevelManager>())
    {
        LevelManager->LoadBattleMap();
    }

    //전투 연결 시 로직
    //StageModule에 SelectedStage를 만들어둚.
    //스테이지에서 시작 누르면 해당 스테이지 데이터를 Selected로 바꿔줌
    //전투 레벨 불러올 때 SelectedStage 데이터로 세팅해줌

}

void ULobbyWidget::OnClickInventory(EUIType InMenuType)
{
    switch (InMenuType)
    {
    case EUIType::PopupInventory:
            ShowInventory();
            break;
    }
}
