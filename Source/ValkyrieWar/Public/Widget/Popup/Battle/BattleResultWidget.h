#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Enum/StateEnums.h"
#include "Data/Table/GameData/RewardDataRow.h"

#include "Widget/Popup/Battle/BattleWinWidget.h"
#include "Widget/Popup/Battle/BattleDefeatWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"

#include "BattleResultWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class VALKYRIEWAR_API UBattleResultWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "BattleResult")
	void SetBattleResult(EBattleState InBattleState);
	UFUNCTION(BlueprintCallable, Category = "BattleResult")
	void UpdateBattleReward();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleWinWidget> Widget_Win = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleDefeatWidget> Widget_Defeat = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackToLobbyButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GoToNextLevelButton = nullptr;

	TObjectPtr<USizeBox> BackToLobbyBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> GoToNextLevelBox = nullptr;

private:
	UPROPERTY()
	EBattleState BattleState = EBattleState::Init;
	UPROPERTY()
	TArray<FRewardDataRow> InRewardList;

private:
	UFUNCTION()
	void HandleBackToLobbyClicked();
	UFUNCTION()
	void HandleGoToNextLevelClicked();

	void BindDelegates();
	void UnbindDelegates();
	void RefreshResultUI();

	bool IsValidResultState(EBattleState InState) const;	
};
