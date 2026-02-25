// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Widget/Popup/Inventory/SellButtonWidget.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UInventoryWidget : public UBaseWidget
{
	GENERATED_BODY()
	// 인벤토리 위젯
	// 아이템 판매 및 사용 기능
	// 모든 아이템 확인 가능

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void OpenUI() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseUI() override;

protected:
	UFUNCTION()
	void FilterReset();
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

	// 현재 필터링된 인벤토리 업데이트 함수
	UFUNCTION()
	void UpdateInventory();

	UFUNCTION()
	void ItemClicked(UObject* InItemData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> InventoryTileView = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close = nullptr;

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
	TObjectPtr<USellButtonWidget> SellButtonWidget = nullptr;

private:
	UInventorySystem* InventorySystem;

	UWorldEventSystem* EventSystem;

	UPROPERTY()
	TArray<UItemData*> CachedItemList;

	// 현재 필터가 적용된 인벤토리 확인용
	EItemGroup CurrentItemGroup = EItemGroup::None;
	EEquipGroup CurrentEquipGroup = EEquipGroup::None;
};
