// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Table/GameData/ItemDataRow.h"
#include "ItemData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UItemData : public UObject
{
	GENERATED_BODY()

public:
	TObjectPtr<FItemDataRow> TableData;
	int32 Amount = 0;

	//SubSystem에 Factory 개념으로 들고있고
	//데이터 세팅해주는게 필요하다...
	//GameInstance에서 테이블 불러와서 가지고있고
	//해당 Row별로 데이터를 세팅해줌(이 ItemData)
	//ItemData에는 테이블 Row 데이터 한개랑
	//Factory에서 아이템 탐색, 이런게 다 가능... 그러면 그냥 구조체로 가지고 있어도..?
};
