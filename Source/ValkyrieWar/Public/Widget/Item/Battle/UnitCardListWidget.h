#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "UnitCardListWidget.generated.h"

class UWidgetAnimation;
class UUnitUpgradeCardWidget;

UCLASS()
class VALKYRIEWAR_API UUnitCardListWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UnitCard")
	void SetVisibleByMode(EInputControlMode InMode);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_In;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_Out;

	// (옵션) WBP에서 카드들을 직접 바인딩해 쓸 수 있게
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitUpgradeCardWidget> Card_1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitUpgradeCardWidget> Card_2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitUpgradeCardWidget> Card_3;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitUpgradeCardWidget> Card_4;

	// 라우팅용 카드 리스트 (생성 방식/바인딩 방식 모두 지원)
	UPROPERTY()
	TArray<TObjectPtr<UUnitUpgradeCardWidget>> Cards;

	bool bIsShown = false;
	bool bBound = false;

	UFUNCTION()
	void OnBattleModeChanged(EInputControlMode InCurrentMode);

	// 업그레이드 상태 수신 (UI 라우팅)
	UFUNCTION()
	void OnUpgradeStateChanged(int32 FamilyId, int32 Level, int32 Cost, bool bAffordable, bool bIsMax);

	void BindDelegates();
	void UnbindDelegates();

	void CacheCardsIfNeeded();
	UUnitUpgradeCardWidget* FindCardByFamilyId(int32 FamilyId) const;

	void ShowList(bool bAnimate);
	void HideList(bool bAnimate);
};
