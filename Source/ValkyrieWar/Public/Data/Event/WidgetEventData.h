// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Game/ItemData.h"
#include "WidgetEventData.generated.h"

/**
 * 위젯 관련 이벤트 구조체
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventoryAmountChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeEquipCharacter, uint64, InCharacterUID);

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FWidgetEventData
{
	GENERATED_BODY()

	//위젯 이벤트 Delegate 모음
public:
	// 인벤토리 Tileview 갱신용
	FOnUpdateInventory OnUpdateInventory;
	// 인벤토리 Tileview Entry Widget 아이템 양 갱신용
	FOnUpdateInventoryAmountChanged OnUpdateInventoryAmountChanged;
	// 장비 장착 및 해제시 캐릭터 정보 장비칸 UI 갱신용
	FOnChangeEquipCharacter OnChangeEquipCharacter;
};
