// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/SaveGame/ItemSaveGame.h"

void UItemSaveGame::AddItem(UItemData* InData)
{
	if (ItemDataList.Contains(InData->GetUID()))
	{
		//있는 아이템이면 DataId랑 UID는 바꿀 필요가 없다..
		FItemDataStruct* ExistData = ItemDataList.Find(InData->GetUID());
		ExistData->Amount = InData->GetAmount();
		ExistData->EquipCharacter = InData->GetEquipCharacter();
	}
	else
	{
		FItemDataStruct AddData;
		AddData.UID = InData->GetUID();
		AddData.DataId = InData->GetTableData().DataId;
		AddData.Amount = InData->GetAmount();
		AddData.EquipCharacter = InData->GetEquipCharacter();

		ItemDataList.Add(InData->GetUID(), AddData);
	}
}

void UItemSaveGame::RemoveItem(uint64 InUID)
{
	if (ItemDataList.Contains(InUID))
	{
		ItemDataList.Remove(InUID);
	}
}
