// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/GoodsModule.h"

void UGoodsModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Goods);
		MakeData();
	}

	SendDataLoadComplete();
}

FGoodsDataRow* UGoodsModule::GetGoodsTable(int32 InKey)
{
	return *TableDataByDataId.Find(InKey);
}

void UGoodsModule::MakeData()
{
	if (DataTable)
	{
		TArray<FGoodsDataRow*> AllRows;
		DataTable->GetAllRows<FGoodsDataRow>(TEXT("GoodsModule_Init"), AllRows);

		for (FGoodsDataRow* Item : AllRows)
		{
			TableDataByDataId.Add(Item->DataId, Item);
		}
	}
}
