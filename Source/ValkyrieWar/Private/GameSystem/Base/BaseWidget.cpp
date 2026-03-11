// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseWidget.h"
#include "Data/Table/GameData/GoodsDataRow.h"

#include "Widget/HUD/TopMenuWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "GameSystem/Library/GameDataHelper.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
}

void UBaseWidget::NativeDestruct()
{
    Super::NativeDestruct();
    EventSystem = nullptr;
}

void UBaseWidget::OpenUI()
{
	bIsOpen = true;
	SetVisibility(ESlateVisibility::Visible);

    CreateTopMenu();
}

void UBaseWidget::CloseUI()
{
	bIsOpen = false;
	SetVisibility(ESlateVisibility::Hidden);
}
void UBaseWidget::RefreshUI()
{
}
void UBaseWidget::OnFocusGained()
{
}

void UBaseWidget::OnFocusLost()
{
}

void UBaseWidget::CreateTopMenu()
{
    if (TopMenuClass)
    {
        FTopMenuDataRow Data;

        if (UGameDataHelper::GetTopMenuData(UIType, GetGameInstance(), Data))
        {
            UTopMenuWidget* TopMenu = CreateWidget<UTopMenuWidget>(this, TopMenuClass);

            if (TopMenu)
            {
                if (RootCanvas)
                {
                    UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(TopMenu);

                    CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.0f));
                    CanvasSlot->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 80.0f));
                }
                TopMenu->SetData(Data);
            }
        }
    }
}
