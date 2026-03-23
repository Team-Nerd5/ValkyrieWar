#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Struct/StageDetailViewData.h"
#include "StageDetailPopupWidget.generated.h"

class UTextBlock;
class UWrapBox;
class UButton;
class UEnemyInfoCardWidget;
class URewardInfoCardWidget;

UCLASS()
class VALKYRIEWAR_API UStageDetailPopupWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;
	virtual void RefreshUI() override;

	void SetStageDetail(const FStageDetailViewData& InDetailData);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleClickClose();

	UFUNCTION()
	void HandleClickStartStage();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Title = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> WrapBox_EnemyList = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> WrapBox_RewardList = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Close = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Start = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage Detail")
	TSubclassOf<UEnemyInfoCardWidget> EnemyInfoCardWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage Detail")
	TSubclassOf<URewardInfoCardWidget> RewardInfoCardWidgetClass;

private:
	UPROPERTY()
	FStageDetailViewData CachedDetailData;
};
