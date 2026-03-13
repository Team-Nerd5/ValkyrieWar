#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "ManaAmpulWidget.generated.h"

class UImage;
class UTextBlock;
class UMaterialInstanceDynamic;

/**
 * 마나 앰플 UI 위젯
 * - Image_ManaFill에 적용된 UI 머티리얼의 FillPercent를 갱신
 * - 텍스트(Current / Max) 갱신
 */
UCLASS()
class VALKYRIEWAR_API UManaAmpulWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	/** 현재 마나만 변경 */
	UFUNCTION(BlueprintCallable, Category = "ManaAmpul")
	void SetCurrentMana(int InCurrentMana, int InMaxMana);

	/** 외부에서 강제로 다시 그리기 */
	UFUNCTION(BlueprintCallable, Category = "ManaAmpul")
	void RefreshManaUI();

protected:
	void CacheDynamicMaterial();
	void UpdateFillMaterial();

private:
	UFUNCTION()
	void OnControllModeChanged(EInputControlMode InMode);

	bool IsManaFull() { return ManaPercent >= 1.f; }

protected:
	/** Fill 머티리얼의 스칼라 파라미터명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManaAmpul")
	FName FillPercentParameterName = TEXT("FillPercent");

	/** 디자이너/초기값 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManaAmpul")
	float CurrentMana = 1000.f;

	/** 너무 낮은 값일 때도 약간 보이게 하고 싶으면 사용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManaAmpul")
	bool bUseMinimumVisualPercent = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManaAmpul", meta = (EditCondition = "bUseMinimumVisualPercent", ClampMin = "0.0", ClampMax = "1.0"))
	float MinimumVisualPercent = 0.05f;

	/** 텍스트를 "현재 / 최대" 형식으로 표시할지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManaAmpul")
	bool bShowMaxManaText = true;

	/** 정수처럼 보이게 표시 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManaAmpul")
	bool bRoundTextValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ModeSwitch")
	bool bIsAutoMode = false;

	/** 머티리얼이 적용된 Fill Image */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_ManaFill = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Glow = nullptr;

	/** 런타임에 가져온 Fill용 MID */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ManaFillMID = nullptr;

private:
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_ModeChange;

	float ManaPercent = 0.f;
};
