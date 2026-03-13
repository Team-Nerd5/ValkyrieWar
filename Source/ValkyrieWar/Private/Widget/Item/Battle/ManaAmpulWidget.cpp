// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/ManaAmpulWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include <GameSystem/Instance/World/WorldEventSystem.h>

void UManaAmpulWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnCurrentManaChanged.AddDynamic(this, &UManaAmpulWidget::SetCurrentMana);
		WorldEventSystem->Battle.OnBattleModeChanged.AddDynamic(this, &UManaAmpulWidget::OnControllModeChanged);
	}
}

void UManaAmpulWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CacheDynamicMaterial();
	RefreshManaUI();
}

void UManaAmpulWidget::SetCurrentMana(int InCurrentMana, int InMaxMana)
{
	CurrentMana = FMath::Clamp((float)InCurrentMana, 0.f, FMath::Max(1.f, (float)InMaxMana));

	ManaPercent = CurrentMana / (float)InMaxMana;

	RefreshManaUI();
}

void UManaAmpulWidget::RefreshManaUI()
{
	if (!ManaFillMID)
	{
		CacheDynamicMaterial();
	}

	UpdateFillMaterial();

	if (IsManaFull())
	{
		Image_Glow->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Glow->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UManaAmpulWidget::CacheDynamicMaterial()
{
	if (!Image_ManaFill)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ManaAmpulWidget] Image_ManaFill is null."));
		return;
	}

	// Brush에 머티리얼이 설정되어 있으면 이를 MID로 가져온다
	ManaFillMID = Image_ManaFill->GetDynamicMaterial();

	if (!ManaFillMID)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ManaAmpulWidget] Failed to get dynamic material from Image_ManaFill."));
	}
}

void UManaAmpulWidget::UpdateFillMaterial()
{
	if (!ManaFillMID)
	{
		return;
	}

	ManaFillMID->SetScalarParameterValue(FillPercentParameterName, ManaPercent);
}

void UManaAmpulWidget::OnControllModeChanged(EInputControlMode InMode)
{
	bIsAutoMode = (InMode == EInputControlMode::Auto) ? true : false;

	if (Anim_ModeChange)
	{
		if (bIsAutoMode)
		{
			PlayAnimationForward(Anim_ModeChange);
		}
		else
		{
			PlayAnimationReverse(Anim_ModeChange);
		}
	}
}
