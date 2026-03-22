// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Widget/Item/Lobby/LobbyMenuWidget.h"
#include "Data/Enum/DataEnums.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULobbyWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void OpenUI() override;

	void ShowStage(int32 InChapter, int32 InStageNum);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> MenuPanel = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULobbyMenuWidget> MenuItemClass = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StageButton = nullptr;

private:
	void ShowInventory();
	void ShowCharacterInfo();
	void ShowStageListPopup();

	void ShowUnitUpgrade();
	void ShowGacha();

	UFUNCTION()
	void ShowStageInternal();

	UFUNCTION()
	void OnClickLobbyMenu(EUIType InMenuType);
};
