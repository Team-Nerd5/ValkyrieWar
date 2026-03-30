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

FOffsetGroupData UEquipTransformModule::GetDataListByValkyrieId(int32 InValkyrieId)
{
	if (TableDataByValkyrieId.Contains(InValkyrieId))
		return TableDataByValkyrieId.FindChecked(InValkyrieId);
	else
		return FOffsetGroupData();
}

void UEquipTransformModule::MakeData()
{
	if (DataTable)
	{
		TArray<FEquipTransformDataRow*> AllRows;
		DataTable->GetAllRows<FEquipTransformDataRow>(TEXT("GachaRandomModule_Init"), AllRows);

		for (FEquipTransformDataRow* Item : AllRows)
		{
			if (!Item) continue;

			if (TableDataByValkyrieId.Contains(Item->ValkyrieId))
			{
				FOffsetGroupData* Data = TableDataByValkyrieId.Find(Item->ValkyrieId);
				Data->DataByItemId.Add(Item->ItemId, *Item);
			}
			else
			{
				FOffsetGroupData Data;
				Data.DataByItemId.Add(Item->ItemId, *Item);
				TableDataByValkyrieId.Add(Item->ValkyrieId, Data);
			}
		}
	}
}
