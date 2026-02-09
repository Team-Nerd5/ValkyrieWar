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
	void Initialize(FItemDataRow* InTableData);

	FORCEINLINE EItemGroup GetItemGroup()
	{
		if (!TableData)
		{
			return EItemGroup::None;
		}

		return TableData->ItemGroup;
	}

private:
	const FItemDataRow* TableData = nullptr;

	UPROPERTY()
	int32 Amount = 0;

};
