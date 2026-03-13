// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/UnitUpgrade/UnitUpgradeWidget.h"

void UUnitUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitUpgradeBox();
}

void UUnitUpgradeWidget::InitUpgradeBox()
{
	if (UpgradeBox1)
		UpgradeBox1->Init(210001);
	if (UpgradeBox2)
		UpgradeBox2->Init(210002);
	if (UpgradeBox3)
		UpgradeBox3->Init(210003);
	if (UpgradeBox4)
		UpgradeBox4->Init(210004);
	if (UpgradeBox5)
		UpgradeBox5->Init(210005);
}
