// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/BattleWidget.h"
#include "Widget/Item/Battle/WallHealthBarWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AllyWallHealthBar && EnemyWallHealthBar)
	{
		AllyWallHealthBar->Init();
		EnemyWallHealthBar->Init();
	}

	if (AttackButton)	
		AttackButton->OnClicked.AddUniqueDynamic(this, &UBattleWidget::OnClickAttack);
	
	if(SkillButton_1)
		SkillButton_1->OnClicked.AddDynamic(this, &UBattleWidget::OnClickSkill_1);

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnSkillSlotReady.AddDynamic(this, &UBattleWidget::SetSkillIcon);
		WorldEventSystem->Widget.OnSkillCooldownStarted.AddDynamic(this, &UBattleWidget::HandleSkillCooldownStarted);
	}

	if (SkillCooldownOverlay_1)
	{
		SkillCooldownOverlay_1->SetVisibility(ESlateVisibility::Hidden);
	}

	if (SkillCooldownText_1)
	{
		SkillCooldownText_1->SetVisibility(ESlateVisibility::Hidden);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			SkillCooldownUiTimerHandle,
			this,
			&UBattleWidget::UpdateSkillCooldownUI,
			0.05f,
			true
		);
	}
}

void UBattleWidget::NativeDestruct()
{
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Widget.OnSkillSlotReady.RemoveDynamic(this, &UBattleWidget::SetSkillIcon);
		WorldEventSystem->Widget.OnSkillCooldownStarted.RemoveDynamic(this, &UBattleWidget::HandleSkillCooldownStarted);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SkillCooldownUiTimerHandle);
	}

	Super::NativeDestruct();
}

FReply UBattleWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (CurrentMode == EInputControlMode::Auto)
		return FReply::Unhandled();

	// 조이스틱 안을 터치했나
	FVector2D LocalTouchPos = JoyPadBGImage->GetCachedGeometry().AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());
	FVector2D BGSize = JoyPadBGImage->GetCachedGeometry().GetLocalSize();

	if (LocalTouchPos.X >= 0.0f && LocalTouchPos.X <= BGSize.X && LocalTouchPos.Y >= 0.0f && LocalTouchPos.Y <= BGSize.Y)
	{
		bIsTouching = true;
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return FReply::Unhandled();
}

FReply UBattleWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (bIsTouching)
	{
		if (JoyPadBGImage)
		{
			 FVector2D BGLocation = JoyPadBGImage->GetCachedGeometry().AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());

			 //배경 이미지 정중앙 좌표 구하기
			 FVector2D BGCenter = JoyPadBGImage->GetCachedGeometry().GetLocalSize() / 2.0f;
			 // 터치 위치에서 중앙 값을 배제해서 정중앙 을 정돈함
			 FVector2D TouchFromCenter = BGLocation - BGCenter;

			 if (TouchFromCenter.Size() > JoyPadArea)
			 {
				 TouchFromCenter = TouchFromCenter.GetSafeNormal() * JoyPadArea;
			 }
			 
			 JoyPadImage->SetRenderTranslation(TouchFromCenter);

			 JoyPadAxis = TouchFromCenter / JoyPadArea;

			 return FReply::Handled();
		}
	}
	return FReply::Unhandled();
}

FReply UBattleWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bIsTouching = false;

	//조이패드 이미지 위치 리셋
	if (JoyPadImage)
		JoyPadImage->SetRenderTranslation(FVector2D::ZeroVector);

	//Axis 리셋
	JoyPadAxis = FVector2D::ZeroVector;

	return FReply::Handled().ReleaseMouseCapture();
}

void UBattleWidget::SetJoyPadVisibility(bool bIsVisible)
{
	CurrentMode = bIsVisible ? EInputControlMode::Manual : EInputControlMode::Auto;


	ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	if (JoyPadImage) JoyPadImage->SetVisibility(NewVisibility);
	if (JoyPadBGImage) JoyPadBGImage->SetVisibility(NewVisibility);
	if (Frame) Frame->SetVisibility(NewVisibility);
	if (Arrows) Arrows->SetVisibility(NewVisibility);

	if (!bIsVisible)
	{
		JoyPadAxis = FVector2D::ZeroVector;
		if (JoyPadImage) JoyPadImage->SetRenderTranslation(FVector2D::ZeroVector);
		bIsTouching = false;
	}
}

void UBattleWidget::SetSkillIcon(int32 InSkillIndex, USkillData* InSkillData)
{
	if (!InSkillData)
	{
		return;
	}

	UTexture2D* LoadedIcon = InSkillData->GetSkillIcon().LoadSynchronous();
	if (!LoadedIcon)
	{
		return;
	}

	switch (InSkillIndex)
	{
	case 0:
		if (SkillIcon_1)
		{
			SkillIcon_1->SetBrushFromTexture(LoadedIcon);
			SkillIcon_1->SetVisibility(ESlateVisibility::Visible);
		}
		break;

	default:
		break;
	}
}

void UBattleWidget::HandleSkillCooldownStarted(int32 InSkillIndex, float InCooldownDuration, float InEndTime)
{
	EnsureCooldownArraySize(InSkillIndex);

	SkillCooldownDurations[InSkillIndex] = InCooldownDuration;
	SkillCooldownEndTimes[InSkillIndex] = InEndTime;

	switch (InSkillIndex)
	{
	case 0:
		if (SkillButton_1)
		{
			SkillButton_1->SetIsEnabled(false);
		}

		if (SkillCooldownOverlay_1)
		{
			SkillCooldownOverlay_1->SetVisibility(ESlateVisibility::Visible);
		}

		if (SkillCooldownText_1)
		{
			SkillCooldownText_1->SetVisibility(ESlateVisibility::Visible);
		}
		break;

	default:
		break;
	}
}

void UBattleWidget::OnClickAttack()
{
	if (EventSystem)
	{
		EventSystem->Valkyrie.OnUseAttack.Broadcast();
	}
}

void UBattleWidget::OnClickSkill_1()
{
	if (EventSystem)
	{
		EventSystem->Valkyrie.OnUseSkill.Broadcast(0);
	}
}

void UBattleWidget::UpdateSkillCooldownUI()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();

	// 현재는 1번 스킬만 처리
	if (SkillCooldownEndTimes.IsValidIndex(0))
	{
		const float Remaining = FMath::Max(0.0f, SkillCooldownEndTimes[0] - CurrentTime);

		if (Remaining <= 0.0f)
		{
			if (SkillButton_1)
			{
				SkillButton_1->SetIsEnabled(true);
			}

			if (SkillCooldownOverlay_1)
			{
				SkillCooldownOverlay_1->SetVisibility(ESlateVisibility::Hidden);
			}

			if (SkillCooldownText_1)
			{
				SkillCooldownText_1->SetVisibility(ESlateVisibility::Hidden);
				SkillCooldownText_1->SetText(FText::GetEmpty());
			}

			SkillCooldownEndTimes[0] = 0.0f;
			SkillCooldownDurations[0] = 0.0f;
		}
		else
		{
			if (SkillButton_1)
			{
				SkillButton_1->SetIsEnabled(false);
			}

			if (SkillCooldownText_1)
			{
				const int32 DisplaySeconds = FMath::CeilToInt(Remaining);
				SkillCooldownText_1->SetText(FText::AsNumber(DisplaySeconds));
				SkillCooldownText_1->SetVisibility(ESlateVisibility::Visible);
			}

			if (SkillCooldownOverlay_1)
			{
				SkillCooldownOverlay_1->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UBattleWidget::EnsureCooldownArraySize(int32 InIndex)
{
	const int32 RequiredSize = InIndex + 1;

	if (SkillCooldownEndTimes.Num() < RequiredSize)
	{
		SkillCooldownEndTimes.SetNumZeroed(RequiredSize);
	}

	if (SkillCooldownDurations.Num() < RequiredSize)
	{
		SkillCooldownDurations.SetNumZeroed(RequiredSize);
	}
}
