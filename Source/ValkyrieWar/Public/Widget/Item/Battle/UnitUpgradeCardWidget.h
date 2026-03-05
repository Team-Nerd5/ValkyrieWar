#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitUpgradeCardWidget.generated.h"

class UButton;
class UBorder;
class UImage;
class UTextBlock;

UCLASS()
class VALKYRIEWAR_API UUnitUpgradeCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// FamilyId/UnitId(=업그레이드 키)
	UFUNCTION(BlueprintCallable, Category = "UnitCard")
	void Init(int32 InUnitId);

	UFUNCTION(BlueprintPure, Category = "UnitCard")
	int32 GetUnitId() const { return UnitId; }

	// UI 갱신은 BP로 위임(아이콘/텍스트는 UMG에서)
	UFUNCTION(BlueprintImplementableEvent, Category = "UnitCard")
	void BP_SetLevel(int32 InLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "UnitCard")
	void BP_SetCost(int32 InCost);

	// 리스트/관리자에서 호출: 상태 한번에 반영
	UFUNCTION(BlueprintCallable, Category = "UnitCard")
	void ApplyUpgradeState(int32 InLevel, int32 InCost, bool bAffordable, bool bIsMax);

	// (옵션) 개별 세팅도 유지
	UFUNCTION(BlueprintCallable, Category = "UnitCard")
	void SetAffordable(bool bAffordable);

	UFUNCTION(BlueprintCallable, Category = "UnitCard")
	void SetMaxLevel(bool bIsMax);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> UpgradeButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> OuterBorder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> CardBorder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> GlowBorder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> UnitIconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ManaIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CostText;

	UPROPERTY()
	int32 UnitId = 0;

	// cached states
	bool bAffordableCached = true;
	bool bIsMaxCached = false;

	UFUNCTION()
	void HandleUpgradeButton();

	void EnsureWidgetTree();

	// Colors + style
	FLinearColor GetBgColor() const;
	FLinearColor GetFrameColor() const;
	FLinearColor GetTextColor() const;
	FLinearColor GetManaColor() const;
	FLinearColor GetWarnColor() const;

	void ApplyCardStyle(bool bIsDesignTime);
	void ApplyStateVisuals();
};
