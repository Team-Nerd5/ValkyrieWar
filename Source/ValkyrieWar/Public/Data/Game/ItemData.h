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
	void Initialize(uint64 InUID, int32 InAmount, FItemDataRow* InTableData);
	void AddAmount(int32 InAmount);

	//예시..
	FORCEINLINE EItemGroup GetItemGroup()
	{
		if (!TableData)
		{
			return EItemGroup::None;
		}

		return TableData->ItemGroup;
	}

	FORCEINLINE const int32 GetAmount() { return Amount; }

	FORCEINLINE const uint64 GetUID() { return UID; }

	FORCEINLINE const uint64 GetEquipCharacterUID()
	{
		if (TableData->ItemGroup == EItemGroup::Weapon || TableData->ItemGroup == EItemGroup::Helmet || TableData->ItemGroup == EItemGroup::Armor)
			return EquipCharacter;
		else
			return 0;
	}
	FORCEINLINE void SetEquipCharacterUID(uint64 InCharacterUID) { EquipCharacter = InCharacterUID; }

private:
	UPROPERTY()
	uint64 UID = 0;

	const FItemDataRow* TableData = nullptr;

	UPROPERTY()
	int32 Amount = 0;

	//장비인 경우 : 장착한 캐릭터 UID
	uint64 EquipCharacter;

};
