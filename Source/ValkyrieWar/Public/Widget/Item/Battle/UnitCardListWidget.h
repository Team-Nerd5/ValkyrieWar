#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "UnitCardListWidget.generated.h"

class UWidgetAnimation;
class UUnitSpawnUpgradeCardWidget;

UCLASS()
class VALKYRIEWAR_API UUnitCardListWidget : public UUserWidget
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

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitSpawnUpgradeCardWidget> Card_1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitSpawnUpgradeCardWidget> Card_2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitSpawnUpgradeCardWidget> Card_3;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitSpawnUpgradeCardWidget> Card_4;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUnitSpawnUpgradeCardWidget> Card_5;

	UPROPERTY()
	TArray<TObjectPtr<UUnitSpawnUpgradeCardWidget>> Cards;

	bool bIsShown = false;
	bool bBound = false;

	UFUNCTION()
	void SetIds(TArray<int32>& InIds);

	UFUNCTION()
	void OnBattleModeChanged(EInputControlMode InCurrentMode);

	// 업그레이드 상태 수신 (MAX 제거)
	UFUNCTION()
	void OnUpgradeStateChanged(int32 FamilyId, int32 Level, int32 Cost, bool bAffordable);

	void BindDelegates();
	void UnbindDelegates();

	void CacheCards();
	UUnitSpawnUpgradeCardWidget* FindCardByFamilyId(int32 FamilyId) const;

	void ShowList(bool bAnimate);
	void HideList(bool bAnimate);
};
