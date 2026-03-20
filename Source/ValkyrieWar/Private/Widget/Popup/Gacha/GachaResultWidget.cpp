// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Widget/Popup/Gacha/GachaResultItemWidget.h"

#include "Components/WrapBox.h"
void UGachaResultWidget::ShowGachaResults(const TArray<int32>& ItemIDs, const TArray<int32>& ItemCounts)
{
	if (!WrapBox_Result || !ItemWidgetClass) return;

	WrapBox_Result->ClearChildren();

	for (int32 i = 0; i < ItemIDs.Num(); ++i)
	{
		FItemDataRow* RandomItem = TestDataTable->FindRow<FItemDataRow>(RowNames[RandomIndex], TEXT("GachaTest"));

		if (NewItem)
		{
			NewItem->SetItemData(nullptr, ItemCounts[i]);

			WrapBox_Result->AddChildToWrapBox(NewItem);
		}
	}
}
