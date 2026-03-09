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
	void SetMenu(TMap<EInventoryFilterType, FString> InMenuNameData);
	void SetMenu(TMap<ECharacterInfoFilterType, FString> InMenuNameData);

	void SetData(TArray<class UItemData*> InItemList);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnTabMenuChanged(int32 InSelectedTab);

	UFUNCTION()
	void OnItemSelected(UObject* InItemData);

	void UpdateFilteredItemList();

	void UpdateButton();

protected:
	ETabType TabType;
	int32 SelectedFilterIndex = 0;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTabMenuWidget> TabMenuClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> TabMenuContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> InventoryTileView = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SellButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EquipButton = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<class UItemData>> CachedItemList;

	UPROPERTY()
	TArray<TObjectPtr<class UItemData>> FilteredItemList;

};
