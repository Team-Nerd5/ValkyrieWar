// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/CharacterInfo/ValkyrieListItemWidget.h"
#include "Data/Game/ValkyrieData.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UValkyrieListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UValkyrieData* Data = Cast<UValkyrieData>(ListItemObject);

	if (Data)
	{
		SetData(Data);
	}
}

void UValkyrieListItemWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	if (SelectedFrame)
	{
		SelectedFrame->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (bIsSelected)
	{
		if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			WorldEventSystem->Widget.OnValkyrieSelected.Broadcast(CachedData);
		}
	}	
}

void UValkyrieListItemWidget::SetStar(EGradeType InGrade)
{
	int32 GradeNum = static_cast<int32>(InGrade);

	if(Star1) Star1->SetVisibility(ESlateVisibility::Visible);
	if(Star2) Star2->SetVisibility((GradeNum > 0) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if(Star3) Star3->SetVisibility((GradeNum > 1) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if(Star4) Star4->SetVisibility((GradeNum > 2) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if(Star5) Star5->SetVisibility((GradeNum > 3) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
void UValkyrieListItemWidget::SetData(UValkyrieData* InValkyrieData)
{
	CachedData = InValkyrieData;

	//이미지
	if (ValkyrieImage)
		ValkyrieImage->SetBrushFromSoftTexture(InValkyrieData->GetIcon());

	//이름세팅
	if (ValkyrieNameText)
		ValkyrieNameText->SetText(FText::FromString(InValkyrieData->GetValkyrieName()));

	SetStar(InValkyrieData->GetGrade());
}
