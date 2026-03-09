// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "TabMenuWidget.generated.h"



class UPanelWidget;
class UTabMenuItemWidget;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTabMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddTab(ETabType InTabType, int32 InFilterIndex, FString InTabName);

	void SetTab(int32 InIndex);

protected:

	UFUNCTION()
	void HandleTabClicked(UTabMenuItemWidget* ClickedTab);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTabMenuItemWidget> TabMenuItemClass;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> TabContainer;

	UPROPERTY()
	TArray<TObjectPtr<UTabMenuItemWidget>> TabItems;
};
