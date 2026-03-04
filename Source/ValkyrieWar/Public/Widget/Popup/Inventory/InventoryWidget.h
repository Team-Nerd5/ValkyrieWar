// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Widget/Popup/Inventory/ActionButtonWidget.h"
#include "Widget/Popup/Inventory/EquipmentSlotWidget.h"
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

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void OpenUI() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseUI() override;

public:
	// 인벤토리 타입 변경 함수
	UFUNCTION(BlueprintCallable)
	void UpdateInventoryType(EUIType InUIType);

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
	void FilterGrowth();
	UFUNCTION()
	void FilterGoods();

	// 현재 필터링된 인벤토리 업데이트 함수
	UFUNCTION()
	void OnUpdateInventory();
	// 선택된 캐릭터가 장착한 장비칸 UI업데이트 함수
	UFUNCTION()
	void OnUpdateEquipmentForUID(uint64 InCharacterUID);
	// 인벤토리 타일뷰 클릭 했을 때 실행될 함수
	UFUNCTION()
	void OnItemClicked(UObject* InItemData);
	// 보유중인 캐릭터 타일뷰 클릭 했을 때 실행될 함수
	//UFUNCTION()
	//void OnCharacterClicked(UObject* InCharacterData);

	UFUNCTION()
	void OnClickClose();

private:
	// UIType에 따라 인벤토리 세팅하기 위한 함수
	void RefreshUIByMode();

	void SortInventory();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> InventoryTileView = nullptr;

	// 보유중인 캐릭터 목록
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UTileView> OwnedCharacterTileView = nullptr;

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
	TObjectPtr<UButton> BackButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UActionButtonWidget> ActionButtonWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEquipmentSlotWidget> EquipmentSlotWidget = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UInventorySystem> InventorySystem;

	UPROPERTY()
	TObjectPtr<UWorldEventSystem> WorldEventSystem;

	UPROPERTY()
	TArray<UItemData*> CachedItemList;

	// 선택된 인벤토리 타입 저장
	EUIType SelectedInventoryType = EUIType::None;

	// 현재 필터가 적용된 인벤토리 확인용
	EItemGroup CurrentItemGroup = EItemGroup::None;
	EEquipGroup CurrentEquipGroup = EEquipGroup::None;

	uint64 TempCharacterUID = 1001001;	// 테스트용 캐릭터UID(케릭터 선택창 구현시 삭제 예정)
};
