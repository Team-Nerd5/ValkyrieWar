// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Game/ItemData.h"
#include "Data/Game/ValkyrieData.h"
#include "WidgetEventData.generated.h"

/**
 * 위젯 관련 이벤트 구조체
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemAmountChanged, uint64, InUID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateCharacterEquipment, uint64, InCharacterUID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabMenuSelected, int32, InSelectedTab);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemSelected, UItemData*, InItemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoodsUpdate, EGoodsType, InGoodsType, uint64, InAmount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValkyrieSelected, UValkyrieData*, InValkyrieData);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInfoWidgetOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInfoWidgetClosed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitUpgrade, EUnitCharacterType, InUnitType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventorySelectedCancel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHiddenInventoryPopup);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipItem, UItemData*, InItemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquipItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickUnEquip, EEquipGroup, InEquipGroup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickEquip, UItemData*, InItemData);


USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FWidgetEventData
{
	GENERATED_BODY()

	//위젯 이벤트 Delegate 모음
public:
	// 인벤토리 Tileview 갱신용
	FOnUpdateInventory OnUpdateInventory;
	// 인벤토리 Tileview Entry Widget 아이템 양 갱신용
	FOnInventoryItemAmountChanged OnInventoryItemAmountChanged;
	// 장비 장착 및 해제시 캐릭터 정보 장비칸 UI 갱신용
	FOnUpdateCharacterEquipment OnUpdateCharacterEquipment;

	FOnTabMenuSelected OnTabMenuSelected;
	FOnInventoryItemSelected OnInventoryItemSelected;

	FOnValkyrieSelected OnValkyrieSelected;

	FOnGoodsUpdate OnGoodsUpdate;

	FOnUnitUpgrade OnUnitUpgrade;

	//캐릭터 정보창 오픈/클로즈 연출
	FOnCharacterInfoWidgetOpened OnCharacterInfoWidgetOpened;
	FOnCharacterInfoWidgetClosed OnCharacterInfoWidgetClosed;

	FOnEquipItem OnEquipItem;
	FOnUnEquipItem OnUnEquipItem;
	FOnClickUnEquip OnClickUnEquip;
	FOnClickEquip OnClickEquip;

	//-----------------------------삭제 예정---------------------------
	// 인벤토리 아이템 선택 취소용
	FOnUpdateInventorySelectedCancel OnUpdateInventorySelectedCancel;
	// 인벤토리에서 띄운 팝업 숨기기용
	FOnHiddenInventoryPopup OnHiddenInventoryPopup;
};
