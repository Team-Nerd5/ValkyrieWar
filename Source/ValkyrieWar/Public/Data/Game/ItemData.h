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

	FORCEINLINE EItemType GetItemType() { return TableData->ItemType; }
	FORCEINLINE EItemGroup GetItemGroup() { return ItemGroup; }
	FORCEINLINE EEquipGroup GetEquipGroup() { return EquipGroup; }
	FORCEINLINE int32 GetAmount() { return Amount; }
	FORCEINLINE int32 GetAttackID() { return TableData ? TableData->AttackId : 0; }
	FORCEINLINE TArray<int32> GetSkillID() { return TableData ? TableData->SkillId : TArray<int32>(); }
	FORCEINLINE uint64 GetEquipCharacter() { return EquipCharacter; }
	FORCEINLINE const uint64 GetUID() { return UID; }
	FORCEINLINE const FItemDataRow* GetTableData() { return TableData; }
	FORCEINLINE bool IsSkeletalWeapon() { return TableData->IsSkeletal; }
	FORCEINLINE TSoftObjectPtr<USkeletalMesh> GetSkeletalMesh() { return TableData->SkeletalMesh; }
	FORCEINLINE TSoftObjectPtr<UStaticMesh> GetStaticMesh() { return TableData->StaticMesh; }

private:
	const FItemDataRow* TableData = nullptr;

	UPROPERTY()
	uint64 UID = 0;

	UPROPERTY()
	int32 Amount = 0;

	//장비인 경우 : 장착한 캐릭터 UID
	UPROPERTY()
	uint64 EquipCharacter;

	UPROPERTY()
	EItemGroup ItemGroup = EItemGroup::None;
	UPROPERTY()
	EEquipGroup EquipGroup = EEquipGroup::None;
};
