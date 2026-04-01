// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "BattleWidget.generated.h"

class UImage;
class UWallHealthBarWidget;
class UButton;
class USkillData;
class UTextBlock;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleWidget : public UBaseWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	FORCEINLINE FVector2D const GetJoyPadAxis() { return JoyPadAxis; }
	void SetJoyPadVisibility(bool bIsVisible);

	UFUNCTION()
	void SetSkillIcon(int32 InSkillIndex, USkillData* InSkillData);

	UFUNCTION()
	void HandleSkillCooldownStarted(int32 InSkillIndex, float InCooldownDuration, float InEndTime);

protected:
	UFUNCTION()
	void OnClickAttack();
	//귀찮아서..우선 번호로
	UFUNCTION()
	void OnClickSkill_1();

	void UpdateSkillCooldownUI();
	void EnsureCooldownArraySize(int32 InIndex);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> JoyPadImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> JoyPadBGImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Frame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Arrows;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWallHealthBarWidget> AllyWallHealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWallHealthBarWidget> EnemyWallHealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AttackButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SkillButton_1 = nullptr;

	// 버튼 안에 배치할 스킬 아이콘 이미지
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SkillIcon_1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SkillCooldownOverlay_1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillCooldownText_1 = nullptr;

private:
	EInputControlMode CurrentMode = EInputControlMode::Manual;

	bool bIsTouching = false;

	FVector2D JoyPadAxis = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	float JoyPadArea = 100.0f;

	TArray<float> SkillCooldownEndTimes;
	TArray<float> SkillCooldownDurations;

	FTimerHandle SkillCooldownUiTimerHandle;
};
