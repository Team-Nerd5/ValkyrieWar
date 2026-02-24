// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/LobbyWidget.h"
#include "Data/Module/ContentsModule.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void ULobbyWidget::OpenUI()
{
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		//1. 탑메뉴 생성해서 세팅?
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
                    NewMenu->SetData(Data->Icon.LoadSynchronous(), Data->Name, Data->OpenPopup);

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
