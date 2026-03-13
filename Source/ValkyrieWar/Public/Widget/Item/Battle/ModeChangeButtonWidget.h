#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "ModeChangeButtonWidget.generated.h"

class UButton;
class UImage;
class UWidgetAnimation;
class UTextBlock;

UCLASS()
class VALKYRIEWAR_API UModeChangeButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ModeSwitch")
	void ToggleAutoMode();

	UFUNCTION(BlueprintPure, Category = "ModeSwitch")
	bool IsAutoMode() const { return bIsAutoMode; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;

protected:
	UFUNCTION()
	void HandleModeSwitchClicked();

	UFUNCTION()
	void RefreshVisual(EInputControlMode InCurrentMode);
	void BindEvents();
	void UnbindEvents();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ModeSwitch = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Background = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Knob = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ModeText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ModeSwitch|Style")
	FText ManualText = FText::FromString(TEXT("Manual"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ModeSwitch|Style")
	FText AutoText = FText::FromString(TEXT("Auto"));

	// 노브 이동 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_ModeChange = nullptr;

protected:
	// 기본 상태
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ModeSwitch")
	bool bIsAutoMode = false;
};
