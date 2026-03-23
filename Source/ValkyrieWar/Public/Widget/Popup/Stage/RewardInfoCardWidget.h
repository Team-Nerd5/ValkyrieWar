#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Struct/RewardViewData.h"
#include "RewardInfoCardWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;

UCLASS()
class VALKYRIEWAR_API URewardInfoCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRewardData(const FRewardViewData& InRewardData);

protected:
	virtual void NativeConstruct() override;

	void RefreshUI();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Root = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_RewardIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Name = nullptr;

private:
	UPROPERTY()
	FRewardViewData CachedRewardData;
};
