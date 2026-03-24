#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "BattlePauseMenuWidget.generated.h"

class UButton;

UCLASS()
class VALKYRIEWAR_API UBattlePauseMenuWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Return;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_BackToLobby;

	UFUNCTION()
	void HandleReturnClicked();

	UFUNCTION()
	void HandleBackToLobbyClicked();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BattlePauseMenu")
	bool bPauseGameWhenOpened = true;
};
