// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ManaDisplayWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UWidgetAnimation;

/**
 * 현재 마나 표시 위젯
 */
UCLASS()
class VALKYRIEWAR_API UManaDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부에서 강제로 갱신하고 싶을 때
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void SetMana(int32 InCurrentMana, int32 InMaxMana);

	UFUNCTION(BlueprintCallable, Category = "Mana")
	void SetCurrentMana(int32 InCurrentMana);

	UFUNCTION(BlueprintCallable, Category = "Mana")
	void SetMaxMana(int32 InMaxMana);

	// “스폰 비용” 같은 기준값을 넣어두면 부족 여부 강조 가능
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void SetRequiredMana(int32 InRequiredMana);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// ===== UMG Bind =====
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ManaText;

	// 부족/충분 상태 변화에 따른 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_LowMana;

	// ===== Cached =====
	UPROPERTY(Transient)
	int32 CurrentMana = 0;

	UPROPERTY(Transient)
	int32 RequiredMana = 0;

	UPROPERTY(Transient)
	bool bIsLowCached = false;

private:
	void RefreshUI();
	void UpdateLowState(bool bLow);

	//UFUNCTION(BlueprintImplementableEvent, Category = "Mana")
	//void BP_OnManaChanged(int32 InCurrentMana, int32 InMaxMana, float InRatio);

	//UFUNCTION(BlueprintImplementableEvent, Category = "Mana")
	//void BP_OnLowManaStateChanged(bool bIsLow);
};
