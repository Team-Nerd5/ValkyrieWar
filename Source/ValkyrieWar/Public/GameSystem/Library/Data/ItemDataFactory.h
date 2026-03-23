// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/Game/ItemData.h"
#include "Data/Table/GameData/ItemDataRow.h"
#include "ItemDataFactory.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UItemDataFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:	
	//id 기반으로 아이템 데이터 생성
	static UItemData* Make(int32 InDataId, int32 Amount);
};
