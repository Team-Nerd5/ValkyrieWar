// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "LobbyMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULobbyMenuWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> MenuButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> MenuName = nullptr;

	EUIType ConnectedUIType = EUIType::None;

public:
	virtual void NativeConstruct() override;

	void SetData(UTexture2D* Icon, FString Name, EUIType InOpenUIType);

protected:
	UFUNCTION()
	void OnClickMenu();
};
