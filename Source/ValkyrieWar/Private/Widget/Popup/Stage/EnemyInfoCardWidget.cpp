// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Stage/EnemyInfoCardWidget.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UEnemyInfoCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshUI();
}

void UEnemyInfoCardWidget::SetEnemyData(const FStageEnemyViewData& InEnemyData)
{
	CachedEnemyData = InEnemyData;
	RefreshUI();
}

void UEnemyInfoCardWidget::RefreshUI()
{
	if (Text_Level_Data)
	{
		Text_Level_Data->SetText(FText::AsNumber(CachedEnemyData.Level));
	}

	if (Border_GradeFrame)
	{
		Border_GradeFrame->SetBrushColor(GetGradeFrameColor(CachedEnemyData.Grade).GetSpecifiedColor());
	}

	if (Image_EnemyIcon)
	{
		UTexture2D* IconTexture = nullptr;

		if (!CachedEnemyData.UnitIcon.IsNull())
		{
			IconTexture = CachedEnemyData.UnitIcon.LoadSynchronous();
		}

		Image_EnemyIcon->SetBrushFromTexture(IconTexture);
	}
}

FSlateColor UEnemyInfoCardWidget::GetGradeFrameColor(EGradeType InGrade) const
{
	switch (InGrade)
	{
	case EGradeType::Common:
		return FSlateColor(FLinearColor(0.45f, 0.45f, 0.45f, 1.f));

	case EGradeType::Uncommon:
		return FSlateColor(FLinearColor(0.15f, 0.8f, 0.25f, 1.f));

	case EGradeType::Rare:
		return FSlateColor(FLinearColor(0.2f, 0.45f, 1.f, 1.f));

	case EGradeType::Unique:
		return FSlateColor(FLinearColor(0.7f, 0.3f, 0.9f, 1.f));

	case EGradeType::Legend:
		return FSlateColor(FLinearColor(1.0f, 0.6f, 0.1f, 1.f));

	default:
		break;
	}

	return FSlateColor(FLinearColor::White);
}
