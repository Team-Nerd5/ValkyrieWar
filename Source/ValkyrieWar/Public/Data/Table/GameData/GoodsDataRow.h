// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "GoodsDataRow.generated.h"

/**
 * 컨텐츠 상위에 표기되는 재화 표기
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FGoodsDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	bool bIsBackButton = true;				//false이면 계정정보
	UPROPERTY(EditAnywhere)
	EGoodsType Type_1 = EGoodsType::None;
	UPROPERTY(EditAnywhere)
	EGoodsType Type_2 = EGoodsType::None;
	UPROPERTY(EditAnywhere)
	EGoodsType Type_3 = EGoodsType::None;
};
