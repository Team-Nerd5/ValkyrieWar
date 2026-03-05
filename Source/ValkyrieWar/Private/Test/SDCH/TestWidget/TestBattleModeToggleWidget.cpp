// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestWidget/TestBattleModeToggleWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"

void UTestBattleModeToggleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ToggleButton)
	{
		ToggleButton->OnClicked.AddDynamic(this, &UTestBattleModeToggleWidget::OnClickToggle);
	}

	//RefreshVisual();
}

void UTestBattleModeToggleWidget::NativeDestruct()
{
	if (ToggleButton)
	{
		ToggleButton->OnClicked.RemoveDynamic(this, &UTestBattleModeToggleWidget::OnClickToggle);
	}

	Super::NativeDestruct();
}

void UTestBattleModeToggleWidget::OnClickToggle()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AValkyrieCharacterController* VPC = Cast<AValkyrieCharacterController>(PC))
		{
			VPC->ToggleControlMode();
		}
	}

	//RefreshVisual();
}

//void UTestBattleModeToggleWidget::RefreshVisual()
//{
//	if (!IconImage) return;
//
//	if (APlayerController* PC = GetOwningPlayer())
//	{
//		if (AValkyrieCharacterController* VPC = Cast<AValkyrieCharacterController>(PC))
//		{
//			IconImage->SetOpacity(1.0f);
//		}
//	}
//}
