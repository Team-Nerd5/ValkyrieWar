// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/ModeChangeButtonWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

void UModeChangeButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Image_Knob)
	{
		// 디자이너에서도 상태가 보이도록
		FWidgetTransform Transform = Image_Knob->GetRenderTransform();
		Image_Knob->SetRenderTransform(Transform);
	}
}

void UModeChangeButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindEvents();
}

void UModeChangeButtonWidget::NativeDestruct()
{
	UnbindEvents();
	Super::NativeDestruct();
}

void UModeChangeButtonWidget::BindEvents()
{
	if (ModeSwitch)
	{
		ModeSwitch->OnClicked.AddDynamic(this, &UModeChangeButtonWidget::HandleModeSwitchClicked);
	}

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleModeChanged.AddDynamic(this, &UModeChangeButtonWidget::RefreshVisual);
	}
}

void UModeChangeButtonWidget::UnbindEvents()
{
	if (ModeSwitch)
	{
		ModeSwitch->OnClicked.RemoveDynamic(this, &UModeChangeButtonWidget::HandleModeSwitchClicked);
	}
}

void UModeChangeButtonWidget::HandleModeSwitchClicked()
{
	ToggleAutoMode();
}

void UModeChangeButtonWidget::ToggleAutoMode()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AValkyrieCharacterController* VPC = Cast<AValkyrieCharacterController>(PC))
		{
			VPC->ToggleControlMode();
		}
	}
}

void UModeChangeButtonWidget::RefreshVisual(EInputControlMode InCurrentMode)
{
	bIsAutoMode = (InCurrentMode == EInputControlMode::Auto ? true : false);
	ModeText->SetText(bIsAutoMode ? AutoText : ManualText);

	if (Image_Knob)
	{
		// 애니메이션이 있으면 재생
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
}
