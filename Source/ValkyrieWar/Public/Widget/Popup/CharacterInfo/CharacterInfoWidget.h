// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Widget/Item/CharacterInfo/InfoEquipItemWidget.h"
#include "CharacterInfoWidget.generated.h"

class UItemData;
class UValkyrieData;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCharacterInfoWidget : public UBaseWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnInitialized() override;

	void InitItemList();

	void InitValkyrieList();

	UFUNCTION()
	void OnItemClicked(UObject* InItemData);

	UFUNCTION()
	void OnInventoryUpdate();

	UFUNCTION()
	void OnTabMenuChanged(int32 InSelectedTab);

	UFUNCTION()
	void OnClickSetMain();

	UFUNCTION()
	void OnValkyrieSelected(UValkyrieData* InSelectedValkyrie);

	UFUNCTION()
	void OnClickUnEquip(EEquipGroup InEquipGroup);
	UFUNCTION()
	void OnClickEquip(UItemData* InItemData);

	void UpdateStat();

	UFUNCTION()
	void OnLongClickItemStart(int32 InItemUID);

	UFUNCTION()
	void OnLongClickItemEnd();

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;
protected:
	//인벤토리
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemListWidget> ItemListWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UValkyrieListWidget> ValkyrieListWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SetMainButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> MainValkyrieIcon = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<ECharacterInfoFilterType, FString> CharacterInfoTabNameData;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoEquipItemWidget> WeaponItem;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoEquipItemWidget> ArmorItem;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoEquipItemWidget> HelmetItem;

	UPROPERTY(EditDefaultsOnly)
	TMap<EEquipGroup, TSoftObjectPtr<UTexture2D>> TypeIcons;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UValkyrieStatWidget> StatWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemInfoWidget> ItemInfoWidget = nullptr;
private:

	UPROPERTY()
	TArray<TObjectPtr<UItemData>> OriginItems;

	ECharacterInfoFilterType CurrentFilterType = ECharacterInfoFilterType::All;

	UPROPERTY()
	TArray<TObjectPtr<UValkyrieData>> OriginValkyires;

	UPROPERTY()
	TObjectPtr<UValkyrieData> MainValkyrie = nullptr;

	UPROPERTY()
	TObjectPtr<UValkyrieData> SelectedValkyrie = nullptr;
};
