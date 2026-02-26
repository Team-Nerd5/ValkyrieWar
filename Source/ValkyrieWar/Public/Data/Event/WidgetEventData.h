// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Game/ItemData.h"
#include "WidgetEventData.generated.h"

/**
 * 위젯 관련 이벤트 구조체
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateEquipment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeEquipCharacter, uint64, InCharacterUID);

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FWidgetEventData
{
	GENERATED_BODY()

	//위젯 이벤트 Delegate 모음
public:
	// 인벤토리 Tileview 갱신용
	FOnUpdateInventory OnUpdateInventory;
	// 장비창 Tileview 갱신용
	FOnUpdateEquipment OnUpdateEquipment;
	// 장비 장착시 인벤토리UI 아이템의 장착한 케릭터UID 갱신용
	FOnChangeEquipCharacter OnChangeEquipCharacter;
};
