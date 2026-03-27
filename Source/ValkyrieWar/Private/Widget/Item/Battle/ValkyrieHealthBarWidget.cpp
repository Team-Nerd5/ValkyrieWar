// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/ValkyrieHealthBarWidget.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "Data/Event/ValkyrieEventData.h"

void UValkyrieHealthBarWidget::NativeConstruct()
{
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Valkyrie.OnValkyrieHpChanged.RemoveAll(this);
		EventSystem->Valkyrie.OnValkyrieHpChanged.AddDynamic(this, &UValkyrieHealthBarWidget::RefreshHp);
	}
}

void UValkyrieHealthBarWidget::NativeDestruct()
{
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Valkyrie.OnValkyrieHpChanged.RemoveAll(this);
	}
}

void UValkyrieHealthBarWidget::RefreshHp(float CurrentHp, float MaxHp)
{
	if (!ProgressBar_HP)
	{
		return;
	}

	const float Percent = MaxHp > 0.f ? CurrentHp / MaxHp : 0.f;
	ProgressBar_HP->SetPercent(Percent);

	if (Text_HP)
	{
		Text_HP->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), FMath::Max(0.0f, CurrentHp), MaxHp)));
	}
}
