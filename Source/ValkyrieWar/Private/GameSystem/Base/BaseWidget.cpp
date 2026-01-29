// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseWidget.h"

void UBaseWidget::OpenUI()
{
	bIsOpen = true;
	SetVisibility(ESlateVisibility::Visible);
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