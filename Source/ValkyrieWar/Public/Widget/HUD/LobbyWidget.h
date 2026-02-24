// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Widget/Item/Lobby/LobbyMenuWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULobbyWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void OpenUI() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> MenuPanel = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULobbyMenuWidget> MenuItemClass = nullptr;
};
