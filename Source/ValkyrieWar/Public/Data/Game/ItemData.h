// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Table/GameData/ItemDataRow.h"
#include "GameSystem/Instance/Game/GameManager.h"
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
	void Equip(uint64 InEquipCharacter);


	FORCEINLINE EItemGroup GetItemGroup()
	{
		if (!TableData)
		{
			return EItemGroup::None;
		}

		return TableData->ItemGroup;
	}

	FORCEINLINE int32 GetAmount() { return Amount; }
	FORCEINLINE int32 GetAttackID() { return TableData ? TableData->AttackId : 0; }
	FORCEINLINE TArray<int32> GetSkillID() { return TableData ? TableData->SkillId : TArray<int32>(); }
	FORCEINLINE uint64 GetEquipCharacter() { return EquipCharacter; }

	FORCEINLINE const uint64 GetUID() { return UID; }

	FORCEINLINE const uint64 GetEquipCharacterUID()
	{
		if (!TableData && !(TableData->ItemGroup == EItemGroup::Armor))
			return;

		return EquipCharacter;
	}
	

private:
	UPROPERTY()
	uint64 UID = 0;

	const FItemDataRow* TableData = nullptr;

	UPROPERTY()
	int32 Amount = 0;

	//장비인 경우 : 장착한 캐릭터 UID
	uint64 EquipCharacter;
};
