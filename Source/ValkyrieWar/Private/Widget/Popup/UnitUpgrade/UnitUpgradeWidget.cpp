// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/UnitUpgrade/UnitUpgradeWidget.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/UnitModule.h"

void UUnitUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Init();
	InitUpgradeBox();
}

void UUnitUpgradeWidget::Init()
{
	if (UpgradeBox1) UpgradeBoxes.Add(UpgradeBox1);
	if (UpgradeBox2) UpgradeBoxes.Add(UpgradeBox2);
	if (UpgradeBox3) UpgradeBoxes.Add(UpgradeBox3);
	if (UpgradeBox4) UpgradeBoxes.Add(UpgradeBox4);
	if (UpgradeBox5) UpgradeBoxes.Add(UpgradeBox5);
}

void UUnitUpgradeWidget::InitUpgradeBox()
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UUnitModule* UnitModule = DataManager->GetUnitModule();
	if (!UnitModule)
		return;

	TArray<int32> UnitDataIds;
	UnitModule->GetOwnedUnitIds(UnitDataIds);

	for (int32 Key : UnitDataIds)
	{
		for (TObjectPtr<UUnitUpgradeBoxWidget> Box : UpgradeBoxes)
		{
			if (!Box)
				break;

			if (Box->GetUnitDataId() != 0)
				continue;
			Box->Init(Key);
			break;
		}
	}
}
