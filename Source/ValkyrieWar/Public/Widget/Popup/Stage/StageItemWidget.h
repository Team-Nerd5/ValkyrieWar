#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Module/StageModule.h"
#include "StageItemWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStageItemClicked, int32, Chapter, int32, StageNum);

UCLASS()
class VALKYRIEWAR_API UStageItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const FStageEntryData& InViewData);
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintAssignable)
	FOnStageItemClicked OnStageItemClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Stage = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StageNum = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Lock = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Border_Selected = nullptr;

	UFUNCTION()
	void HandleClicked();

	void RefreshVisual();

private:
	FStageEntryData ViewData;
	bool bSelected = false;
};
