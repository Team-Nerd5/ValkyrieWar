// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/CharacterInfo/InfoEquipItemWidget.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Components/Image.h"
#include "Components/Button.h"

void UInfoEquipItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UnEquipButton)
		UnEquipButton->OnClicked.AddDynamic(this, &UInfoEquipItemWidget::UnEquipItem);
}

void UInfoEquipItemWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (UnEquipButton)
		UnEquipButton->OnClicked.RemoveDynamic(this, &UInfoEquipItemWidget::UnEquipItem);
}

void UInfoEquipItemWidget::InitEquip(EEquipGroup InEquipGroup, TSoftObjectPtr<UTexture2D> InIcon)
{
	EquipGroup = InEquipGroup;

	if (ItemIcon)
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);

	if (TypeIcon)
	{
		TypeIcon->SetBrushFromSoftTexture(InIcon);
	}
}

void UInfoEquipItemWidget::SetEquip(TSoftObjectPtr<UTexture2D> InIcon)
{
	if (!InIcon.IsNull())
	{
		ItemIcon->SetBrushFromSoftTexture(InIcon);
	}

	if (TypeIcon)
		TypeIcon->SetVisibility(InIcon.IsNull() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if(ItemIcon)
		ItemIcon->SetVisibility(InIcon.IsNull() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

void UInfoEquipItemWidget::UnEquipItem()
{
	if (ItemIcon->GetVisibility() != ESlateVisibility::Visible)
		return;

	//이벤트로 캐릭터 위젯에 전달(EquipGroup)
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnClickUnEquip.Broadcast(EquipGroup);
	}
	
	//눌리면 해제
	SetEquip(nullptr);
}
