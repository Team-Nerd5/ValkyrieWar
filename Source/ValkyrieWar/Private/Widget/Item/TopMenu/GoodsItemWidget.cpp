// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Item/TopMenu/GoodsItemWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/Game/DataManager.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "Data/Table/GameData/GoodsDataRow.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"


void UGoodsItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorldEventSystem* Event = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		Event->Widget.OnGoodsUpdate.AddDynamic(this, &UGoodsItemWidget::OnGoodsChange);
	}
}

void UGoodsItemWidget::NativeDestruct()
{
	if (UWorldEventSystem* Event = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		Event->Widget.OnGoodsUpdate.RemoveDynamic(this, &UGoodsItemWidget::OnGoodsChange);
	}
	Super::NativeDestruct();
}

void UGoodsItemWidget::OnGoodsChange(EGoodsType InGoodsType, uint64 InAmount)
{
	if (CurrentType == InGoodsType)
	{
		if (AmountText)
		{
			AmountText->SetText(FText::AsNumber(InAmount));
		}
	}
}

void UGoodsItemWidget::SetData(EGoodsType InGoodsType)
{
	CurrentType = InGoodsType;
	if (USaveManager* Save = GetGameInstance()->GetSubsystem<USaveManager>())
	{
		 if (AmountText)
		 {
			 AmountText->SetText(FText::AsNumber(Save->GetGoodsValue(CurrentType)));
		 }
	}

	if (UDataManager* Data = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		FGoodsDataRow Table = Data->GetGoodsModule()->GetTableData(CurrentType);
		if (Icon)
		{
			Icon->SetBrushFromSoftTexture(Table.Icon);
		}
	}
}
