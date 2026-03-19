// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "TargetingDataRow.generated.h"

/**
 * 
 */
//공격 시 공격 타겟을 찾기위한 타겟팅 규칙 데이터
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FTargetingDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	ETargetingPriority TargetPriority01 = ETargetingPriority::Close;			//공격타겟 우선순위 1
	UPROPERTY(EditAnywhere)
	ETargetingPriority TargetPriority02 = ETargetingPriority::Basement;			//공격타겟 우선순위 2

	//스플래시 : 타겟 수치가 존재하면 메인 타겟 주변 SpashRange 내의 타겟 수에게도 데미지
	//데미지 감소 등은 고려하지 않음... 그냥 한명 때리는데 주변에 더 맞출거냐로 생각(타겟 최대 수 증가 느낌)
	UPROPERTY(EditAnywhere)
	int32 SplashTargetAmount = 0;
	UPROPERTY(EditAnywhere)
	float SplashRange = 100.0f;		//타겟이 2이상이면 메인타겟 제외하고는 주변 타겟을 찾음
};
