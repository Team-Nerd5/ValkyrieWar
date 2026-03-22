// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Gacha/GachaResultItemWidget.h"
#include "Widget/Popup/Gacha/GachaResultWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"




void UGachaResultItemWidget::SetItemData(UTexture2D* IconTexture, int32 ItemCount)
{
    if (Image_Icon && IconTexture)
    {
        Image_Icon->SetBrushFromTexture(IconTexture);
    }

    if (TextBlock_Count)
    {
        FString CountString = FString::Printf(TEXT("x%d"), ItemCount);
        TextBlock_Count->SetText(FText::FromString(CountString));
    }
}
