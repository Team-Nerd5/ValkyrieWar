// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/EquipTransformModule.h"

void UEquipTransformModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::EquipTransfrom);
		MakeData();
	}

	SendDataLoadComplete();
}

FOffsetGroupData UEquipTransformModule::GetDataListByValkyrieId(int32 InGroupId)
{
	if (TableDataByGroupId.Contains(InGroupId))
		return TableDataByGroupId.FindChecked(InGroupId);
	else
		return FOffsetGroupData();
}

void UEquipTransformModule::MakeData()
{
	if (DataTable)
	{
		TArray<FEquipTransformDataRow*> AllRows;
		DataTable->GetAllRows<FEquipTransformDataRow>(TEXT("EquipTransform_Init"), AllRows);

		for (FEquipTransformDataRow* Item : AllRows)
		{
			if (!Item) continue;

			if (TableDataByGroupId.Contains(Item->TransformGroupId))
			{
				FOffsetGroupData* Data = TableDataByGroupId.Find(Item->TransformGroupId);
				Data->DataByItemId.Add(Item->ItemId, *Item);
			}
			else
			{
				FOffsetGroupData Data;
				Data.DataByItemId.Add(Item->ItemId, *Item);
				TableDataByGroupId.Add(Item->TransformGroupId, Data);
			}
		}
	}
}
