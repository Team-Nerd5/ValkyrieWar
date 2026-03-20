#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Enum/StateEnums.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"

#include "Widget/Popup/Battle/BattleRewardWidget.h"

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
	void SetBattleReward(TArray<int32> InRewardList);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackToLobbyButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GoToNextLevelButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> VictoryBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> LoseBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> BackToLobbyBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> GoToNextLevelBox = nullptr;


private:
	UPROPERTY()
	EBattleState BattleState = EBattleState::Init;

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
