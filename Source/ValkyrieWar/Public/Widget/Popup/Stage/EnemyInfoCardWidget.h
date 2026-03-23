#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Struct/StageEnemyViewData.h"
#include "EnemyInfoCardWidget.generated.h"

class UImage;
class UBorder;
class UTextBlock;

UCLASS()
class VALKYRIEWAR_API UEnemyInfoCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetEnemyData(const FStageEnemyViewData& InEnemyData);

protected:
	virtual void NativeConstruct() override;

	void RefreshUI();
	FSlateColor GetGradeFrameColor(EGradeType InGrade) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_EnemyIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_GradeFrame = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_LevelBadge = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Level_Data = nullptr;

private:
	UPROPERTY()
	FStageEnemyViewData CachedEnemyData;
};
