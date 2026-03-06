#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Enum/StateEnums.h"
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

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackToLobbyButton = nullptr;

private:
	UPROPERTY()
	EBattleState BattleState = EBattleState::Init;

private:
	UFUNCTION()
	void HandleBackToLobbyClicked();

	void BindDelegates();
	void UnbindDelegates();
	void RefreshResultUI();

	FText GetHeaderText() const;
	bool IsValidResultState(EBattleState InState) const;
};
