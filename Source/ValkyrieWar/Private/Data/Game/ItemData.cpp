// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/ItemData.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Library/GameDataHelper.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UItemData::Initialize(uint64 InUID, int32 InAmount, FItemDataRow InTableData)
{
	UID = InUID;
	Amount = InAmount;
	TableData = InTableData;

	ItemGroup = UGameDataHelper::GetItemGroup(InTableData.ItemType);
	EquipGroup = UGameDataHelper::GetEquipGroup(InTableData.ItemType);
}

void UItemData::AddAmount(int32 InAmount)
{
	Amount += InAmount;
}

void UItemData::Equip(uint64 InEquipCharacter)
{
	EquipCharacter = InEquipCharacter;
}

//기본무기용 데이터 만들어주기
void UItemData::MakeData(FItemDataRow InTableData)
{
	Amount = 1;
	TableData = InTableData;

	ItemGroup = UGameDataHelper::GetItemGroup(InTableData.ItemType);
	EquipGroup = UGameDataHelper::GetEquipGroup(InTableData.ItemType);
}
