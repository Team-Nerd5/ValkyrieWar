// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "ItemListWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UItemListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//inventorywidget에서 SetData(ETabType::Inventory, InventoryTabData); 이런식으로 호출
	void SetData(ETabType InTabType, TMap<int32, FString> InMenuNameData);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnTabMenuChanged(ETabType InTabType, int32 InSelectedTab);
protected:
	ETabType TabType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTabMenuWidget> TabMenuClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> TabMenuContainer;
};
