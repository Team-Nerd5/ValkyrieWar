#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "BattleTimerWidget.generated.h"

class UTextBlock;

UCLASS()
class VALKYRIEWAR_API UBattleTimerWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeText;

	UPROPERTY(EditAnywhere, Category = "Timer")
	FLinearColor NormalColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Timer")
	FLinearColor WarningColor = FLinearColor(1.f, 0.1f, 0.1f, 1.f); // 빨강

	// 60초 이하 빨강, 10초 이하 깜빡임
	UPROPERTY(EditAnywhere, Category = "Timer")
	int32 WarningSeconds = 60;

	UPROPERTY(EditAnywhere, Category = "Timer")
	int32 BlinkSeconds = 10;

	UPROPERTY(EditAnywhere, Category = "Timer")
	float BlinkInterval = 0.2f; // 깜빡임 속도

private:
	bool bBlinking = false;
	bool bBlinkOn = true;

	FTimerHandle BlinkTimerHandle;

	UFUNCTION()
	void OnTimeChanged(float InCurrentTime);

	void UpdateTimeTextAndColor(float InSeconds);

	void StartBlink();
	void StopBlink();
	void ToggleBlink();
};
