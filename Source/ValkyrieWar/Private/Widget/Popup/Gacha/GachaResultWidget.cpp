// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Widget/Item/Gacha/GachaResultItemWidget.h"

#include "Components/WrapBox.h"
void UGachaResultWidget::ShowGachaResults(const TArray<int32>& ItemIDs, const TArray<int32>& ItemCounts)
{
	if (!WrapBox_Result || !ItemWidgetClass) return;
	
	WrapBox_Result->ClearChildren();

}
