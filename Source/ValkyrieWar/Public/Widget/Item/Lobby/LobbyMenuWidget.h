// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/DataEnums.h"
#include "Data/Table/GameData/ContentsDataRow.h"
#include "LobbyMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuButtonClicked, EUIType, InUIType);
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

	EUIType CurrentMenuType = EUIType::None;

public:
	UPROPERTY()
	FOnMenuButtonClicked OnMenuButtonClicked;

public:
	virtual void NativeConstruct() override;

	void SetData(FContentsDataRow InTableData);

protected:
	UFUNCTION()
	void OnClickMenu();
};
