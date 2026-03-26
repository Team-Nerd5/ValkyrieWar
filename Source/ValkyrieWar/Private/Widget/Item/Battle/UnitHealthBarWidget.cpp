// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/UnitHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UUnitHealthBarWidget::SetTeamColor(const FLinearColor& InColor)
{
	if (ProgressBar_HP)
	{
		ProgressBar_HP->SetFillColorAndOpacity(InColor);
	}
}

void UUnitHealthBarWidget::SetHp(float CurrentHp, float MaxHp)
{
	if (!ProgressBar_HP)
	{
		return;
	}

	const float Percent = (MaxHp > 0.f) ? (CurrentHp / MaxHp) : 0.f;
	ProgressBar_HP->SetPercent(Percent);
}
