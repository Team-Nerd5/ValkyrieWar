// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/TopMenuWidget.h"
#include "Widget/Item/TopMenu/GoodsItemWidget.h"

#include "GameSystem/Instance/Game/UIManager.h"

#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"


void UTopMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UTopMenuWidget::OnClickBackButton);
	}
}

void UTopMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (BackButton)
	{
		BackButton->OnClicked.RemoveDynamic(this, &UTopMenuWidget::OnClickBackButton);
	}
}

void UTopMenuWidget::SetData(FGoodsDataRow* InData)
{
	if (BackButtonPanel)
	{
		BackButtonPanel->SetVisibility(InData->bIsBackButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (InData->bIsBackButton == false)
	{
		//계정정보 생성
	}

	//재화 생성
	//재화 개수는 세팅하면서 가져오기
	if (InData->Type_1 != EGoodsType::None)
	{
		UGoodsItemWidget* Item1 = CreateWidget<UGoodsItemWidget>(this, GoodsItemClass);
		if (Item1)
		{
			GoodsItemRoot->AddChildToHorizontalBox(Item1);
			GoodsWidgets.Add(InData->Type_1, Item1);
		}
	}
	if (InData->Type_2 != EGoodsType::None)
	{
		UGoodsItemWidget* Item2 = CreateWidget<UGoodsItemWidget>(this, GoodsItemClass);
		if (Item2)
		{
			Item2->SetPadding(FMargin(30, 0, 0, 0));
			GoodsItemRoot->AddChildToHorizontalBox(Item2);
			GoodsWidgets.Add(InData->Type_2, Item2);
		}
	}
	if (InData->Type_3 != EGoodsType::None)
	{
		UGoodsItemWidget* Item3 = CreateWidget<UGoodsItemWidget>(this, GoodsItemClass);
		if (Item3)
		{
			Item3->SetPadding(FMargin(30, 0, 0, 0));
			GoodsItemRoot->AddChildToHorizontalBox(Item3);
			GoodsWidgets.Add(InData->Type_3, Item3);
		}
	}
}

void UTopMenuWidget::OnClickBackButton()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->CloseTopPopupUI();
	}
}
