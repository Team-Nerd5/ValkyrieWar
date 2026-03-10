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

	void InitFilterIndex(int32 InIndex);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void UpdateFilteredItemList();

	void UpdateButton();

	void SortInventory();

	UFUNCTION()
	void OnItemSelected(UItemData* InItemData);

	UFUNCTION()
	void OnClickSellItem();

	UFUNCTION()
	void OnClickEquipItem();
protected:
	ETabType TabType;
	int32 SelectedFilterIndex = 0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTabMenuWidget> TabMenu;

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


	UPROPERTY()
	TObjectPtr<class UItemData> SelectedItem = nullptr;
};
