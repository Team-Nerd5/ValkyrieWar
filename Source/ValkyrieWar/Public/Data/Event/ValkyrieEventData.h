// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ValkyrieEventData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseSkill, int32, InIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseAttack);

/**
 * 발키리 캐릭터 관련 이벤트 데이터 구조체
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FValkyrieEventData
{
	GENERATED_BODY()
public:
	FOnUseSkill OnUseSkill;
	FOnUseAttack OnUseAttack;
};
