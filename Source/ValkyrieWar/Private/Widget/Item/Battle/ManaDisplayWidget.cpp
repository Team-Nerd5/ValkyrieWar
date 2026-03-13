// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Item/Battle/ManaDisplayWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

void UManaDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnCurrentManaChanged.AddDynamic(this, &UManaDisplayWidget::SetCurrentMana);
		WorldEventSystem->Battle.OnBattleModeChanged.AddDynamic(this, &UManaDisplayWidget::OnControllModeChanged);
	}

	RefreshUI();
}

void UManaDisplayWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UManaDisplayWidget::SetMana(int32 InCurrentMana, int32 InMaxMana)
{
	RefreshUI();
}

void UManaDisplayWidget::SetCurrentMana(int32 InCurrentMana, int32 InMaxMana)
{
	CurrentMana = InCurrentMana;
	ManaProgress = (float)InCurrentMana / (float)InMaxMana;
	RefreshUI();
}

void UManaDisplayWidget::SetMaxMana(int32 InMaxMana)
{
	RefreshUI();
}

void UManaDisplayWidget::SetRequiredMana(int32 InRequiredMana)
{
	RequiredMana = FMath::Max(0, InRequiredMana);
	RefreshUI();
}

void UManaDisplayWidget::RefreshUI()
{
	// Text: "35 / 100"
	if (ManaText)
	{
		ManaText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CurrentMana)));
	}

	// Bar: 0~1
	if (ManaBar)
	{
		ManaBar->SetPercent(FMath::Clamp(ManaProgress, 0.f, 1.f));
	}

	// Low state 판단 (RequiredMana 기준이 있으면 그걸, 없으면 단순 임계치)
	bool bLow = false;
	if (RequiredMana > 0)
	{
		bLow = (CurrentMana < RequiredMana);
	}
	else
	{
		 //기본: 20% 미만이면 low
		//bLow = (Ratio < 0.2f);
	}

	UpdateLowState(bLow);

	// BP 훅
	//BP_OnManaChanged(CurrentMana);
}

void UManaDisplayWidget::UpdateLowState(bool bLow)
{
	if (bIsLowCached == bLow) return;
	bIsLowCached = bLow;

	// 애니메이션은 선택: Low 들어갈 때만 재생
	if (bIsLowCached && Anim_LowMana)
	{
		PlayAnimation(Anim_LowMana, 0.f, 0); // 무한루프는 BP에서 설정 추천
	}
	else
	{
		// Anim이 있다면 정지 (없어도 안전)
		if (Anim_LowMana)
		{
			StopAnimation(Anim_LowMana);
		}
	}

	//BP_OnLowManaStateChanged(bIsLowCached);
}

void UManaDisplayWidget::OnControllModeChanged(EInputControlMode InMode)
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
