// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Stage/StageItemWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"

void UStageItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Stage)
	{
		Button_Stage->OnClicked.AddDynamic(this, &UStageItemWidget::HandleClicked);
	}

	RefreshVisual();
}

void UStageItemWidget::Setup(const FStageEntryData& InViewData)
{
	ViewData = InViewData;
	RefreshVisual();
}

void UStageItemWidget::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	RefreshVisual();
}

void UStageItemWidget::RefreshVisual()
{
	if (Text_StageNum)
	{
		Text_StageNum->SetText(FText::AsNumber(ViewData.StageNum));
	}

	if (Image_Lock)
	{
		Image_Lock->SetVisibility(
			ViewData.bLocked ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed
		);
	}

	if (Button_Stage)
	{
		Button_Stage->SetIsEnabled(!ViewData.bLocked);
	}

	//if (Border_Selected)
	//{
	//	Border_Selected->SetVisibility(
	//		bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed
	//	);
	//}
}

void UStageItemWidget::HandleClicked()
{
	if (ViewData.bLocked)
	{
		return;
	}

	OnStageItemClicked.Broadcast(ViewData.Chapter, ViewData.StageNum);
}
