// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/ItemData.h"

void UItemData::Initialize(uint64 InUID, int32 InAmount, FItemDataRow* InTableData)
{
	UID = InUID;
	Amount = InAmount;
	TableData = InTableData;
}

void UItemData::AddAmount(int32 InAmount)
{
	Amount += InAmount;
}
