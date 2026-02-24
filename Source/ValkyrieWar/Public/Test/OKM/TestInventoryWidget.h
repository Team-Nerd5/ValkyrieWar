// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "Test/OKM/TestPopupWidget.h"
#include "Test/OKM/TestEquipmentWidget.h"

#include "TestInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTestInventoryWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void OpenUI() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseUI() override;

	UFUNCTION()
	void FilterReset();
	// 인벤토리창
	UFUNCTION()
	void FilterInventory();
	// 장비창
	UFUNCTION()
	void FilterEquipment();

	UFUNCTION()
	void FilterWeapon();
	UFUNCTION()
	void FilterArmor();
	UFUNCTION()
	void FilterHelmet();
	UFUNCTION()
	void FIlterGrowth();
	UFUNCTION()
	void FIlterGoods();

	UFUNCTION()
	void ItemClicked(UObject* InItemData);

	// 장착한 장비 UI 정보 갱신용
	UFUNCTION()
	void UpdateEquipmentUi(uint64 InCharacterUID, EEquipGroup InEquipGroup);

private:
	void SortInventory();

public:
	FORCEINLINE void SetCurrentCharacterUID(uint64 inUID) { CurrentCharacterUID = inUID; }
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Money = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TileView = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Inventory = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Equipment = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterReset = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterWeapon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterArmor = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterHelmet = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterGrowth = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterGoods = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTestPopupWidget> PopupWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTestEquipmentWidget> WeaponWidget = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTestEquipmentWidget> HelmetWidget = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTestEquipmentWidget> ArmorWidget = nullptr;
	

private:
	UInventorySystem* InventorySystem;

	UWorldEventSystem* EventSystem;

	UPROPERTY()
	TArray<UItemData*> CachedItemList;

	EUIType UIType;

	uint64 CurrentCharacterUID = 0;
};
