// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Tools/Tab/TabMenuItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"


void UTabMenuItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (TabButton)
	{
		TabButton->OnClicked.AddDynamic(this, &UTabMenuItemWidget::OnTabButtonClicked);
	}
}

void UTabMenuItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (TabButton)
	{
		TabButton->OnClicked.RemoveDynamic(this, &UTabMenuItemWidget::OnTabButtonClicked);
	}
	OnTabItemSelected.Clear();
}

void UTabMenuItemWidget::SetTabSelected(bool bSelected)
{
	bIsOn = bSelected;

	if (ButtonImage && PressedImage && NormalImage)
	{
		UTexture2D* ButtonTexture = bIsOn ? PressedImage : NormalImage;
		ButtonImage->SetBrushFromTexture(ButtonTexture);
	}
}

void UTabMenuItemWidget::SetTab(ETabType InTabType, int32 InFilterIndex, FString InTabName)
{
	TabType = InTabType;

	if (TabName)
	{
		TabName->SetText(FText::FromString(InTabName));
	}

	FilterIndex = InFilterIndex;
}

void UTabMenuItemWidget::OnTabButtonClicked()
{
	bIsOn = !bIsOn;

	if (bIsOn)
	{
		//다른 탭 버튼 꺼주기 위한 이벤트
		OnTabItemSelected.Broadcast(this);
		if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			//누른 필터 정보를 아이템 리스트로 보내는 이벤트
			EventSystem->Widget.OnTabMenuSelected.Broadcast(FilterIndex);
		}
	}
}
