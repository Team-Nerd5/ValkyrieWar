// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/UnitUpgrade/UnitUpgradeWidget.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"

#include "Components/HorizontalBoxSlot.h"

#include "Data/Module/UnitModule.h"

void UUnitUpgradeWidget::OpenUI()
{
	UIType = EUIType::PopupUnitUpgrade;

	Super::OpenUI();
}

void UUnitUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitUpgradeBox();
}

void UUnitUpgradeWidget::InitUpgradeBox()
{
	if (!UpgradeWidgetClass)
		return;
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UUnitModule* UnitModule = DataManager->GetUnitModule();
	if (!UnitModule)
		return;

	// 유닛 목록 초기화
	if (UnitListBox)
		UnitListBox->ClearChildren();
	UpgradeBoxes.Empty();

	TArray<int32> OwnedUnitIds;
	UnitModule->GetOwnedUnitIds(OwnedUnitIds);

	for (const int32& UnitId : OwnedUnitIds)
	{
		if (UnitId == 0)
			break;

		UUnitUpgradeBoxWidget* NewUpgradeBox = CreateWidget<UUnitUpgradeBoxWidget>(this, UpgradeWidgetClass);
		if(NewUpgradeBox)
		{
			// 유닛 업그레이드 박스 세팅
			NewUpgradeBox->Init(UnitId);
			UpgradeBoxes.Add(NewUpgradeBox);

			if (UnitListBox)
			{
				// 위젯 박스 설정
				UHorizontalBoxSlot* BoxSlot = UnitListBox->AddChildToHorizontalBox(NewUpgradeBox);
				if (BoxSlot)
				{
					FSlateChildSize SizeRule;
					SizeRule.SizeRule = ESlateSizeRule::Fill;
					BoxSlot->SetSize(SizeRule);

					BoxSlot->SetHorizontalAlignment(HAlign_Center);
					BoxSlot->SetVerticalAlignment(VAlign_Center);
				}
			}
		}
	}
}
