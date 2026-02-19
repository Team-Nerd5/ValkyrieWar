// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetEventData.generated.h"

/**
 * 위젯 관련 이벤트 구조체
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeItemAmount);

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FWidgetEventData
{
	GENERATED_BODY()

	//위젯 이벤트 Delegate 모음
public:
	// 인벤토리내 아이템 수 변화 시 UI 초기화용
	FOnChangeItemAmount OnChangeItemAmount;
};
