// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetEventData.generated.h"

/**
 * 위젯 관련 이벤트 구조체
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeItemAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeEquipCharacter);

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FWidgetEventData
{
	GENERATED_BODY()

	//위젯 이벤트 Delegate 모음
public:
	// 아이템 수 변화 시 인벤토리UI 아이템의 Amount 갱신용
	FOnChangeItemAmount OnChangeItemAmount;
	// 장비 장착시 인벤토리UI 아이템의 장착한 케릭터UID 갱신용
	FOnChangeEquipCharacter OnChangeEquipCharacter;
};
