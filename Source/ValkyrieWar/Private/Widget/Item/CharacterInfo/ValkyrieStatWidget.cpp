// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/CharacterInfo/ValkyrieStatWidget.h"
#include "Components/TextBlock.h"

void UValkyrieStatWidget::SetStatValue(FStatValueData InCharacterStat, FStatValueData InWeaponStat)
{
	if (AttackValueText)
		AttackValueText->SetText(FText::Format(FText::FromString(TEXT("{0} (+{1})")), static_cast<int32>(InCharacterStat.Attack), static_cast<int32>(InWeaponStat.Attack)));

	if (DefenceValueText)
		DefenceValueText->SetText(FText::Format(FText::FromString(TEXT("{0} (+{1})")), static_cast<int32>(InCharacterStat.Defence), static_cast<int32>(InWeaponStat.Defence)));

	if (HealthValueText)
		HealthValueText->SetText(FText::Format(FText::FromString(TEXT("{0} (+{1})")), static_cast<int32>(InCharacterStat.Health), static_cast<int32>(InWeaponStat.Health)));
}
