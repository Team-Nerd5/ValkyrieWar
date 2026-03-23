#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Module/StageModule.h"
#include "StageListPanelWidget.generated.h"

class UButton;
class UTextBlock;
class UUniformGridPanel;
class UStageItemWidget;
class UStageModule;
class UStageInfoModule;
class UStageRewardModule;
class UUnitModule;
class UItemModule;
class UStageDetailPopupWidget;
struct FStageDetailViewData;
struct FStageEnemyViewData;
struct FRewardViewData;

UCLASS()
class VALKYRIEWAR_API UStageListPanelWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;
	virtual void RefreshUI() override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_PrevChapter = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_NextChapter = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Chapter = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_StageList = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TSubclassOf<UStageItemWidget> StageItemWidgetClass = nullptr;

	UFUNCTION()
	void HandlePrevChapter();

	UFUNCTION()
	void HandleNextChapter();

	UFUNCTION()
	void HandleStageItemClicked(int32 InChapter, int32 InStageNum);

	void RefreshChapterHeader();
	void RefreshChapterButtons();
	void RebuildStageItems();
	void ClearStageItems();

	UStageModule* GetStageModule() const;
	UStageInfoModule* GetStageInfoModule() const;
	UStageRewardModule* GetStageRewardModule() const;
	UUnitModule* GetUnitModule() const;
	UItemModule* GetItemModule() const;

private:
	bool BuildStageEnemyViewData(
		int32 InChapter,
		int32 InStageNum,
		TArray<FStageEnemyViewData>& OutEnemies,
		int32& OutEnemyLevel) const;

	bool BuildStageRewardData(
		int32 InChapter,
		int32 InStageNum,
		TArray<FRewardViewData>& OutRewards) const;

	void OpenStageDetailPopup(const FStageDetailViewData& InDetailViewData);

private:
	int32 CurrentChapter = 0;
	int32 SelectedChapter = 0;
	int32 SelectedStageNum = 0;

	UPROPERTY()
	TArray<TObjectPtr<UStageItemWidget>> SpawnedStageItems;
};
