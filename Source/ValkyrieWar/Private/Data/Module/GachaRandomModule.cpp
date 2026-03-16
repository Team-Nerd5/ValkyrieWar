// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/GachaRandomModule.h"


void UGachaRandomModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Goods);
		MakeData();
	}

	SendDataLoadComplete();
}
TMap<EGradeType, FGachaRandomDataRow> UGachaRandomModule::GetRandomData(int32 InGroupId)
{
	if (TableDataByGroupId.Contains(InGroupId))
	{
		return TableDataByGroupId.FindChecked(InGroupId).RandomData;
	}

	return TMap<EGradeType, FGachaRandomDataRow>();
}
void UGachaRandomModule::MakeData()
{
	if (DataTable)
	{
		TArray<FGachaRandomDataRow*> AllRows;
		DataTable->GetAllRows<FGachaRandomDataRow>(TEXT("GachaRandomModule_Init"), AllRows);

		for (FGachaRandomDataRow* Item : AllRows)
		{
			if (!Item) continue;

			if (TableDataByGroupId.Contains(Item->GroupId))
			{
				TableDataByGroupId.Find(Item->GroupId)->RandomData.Add(Item->ResultGrade, *Item);
			}
			else
			{
				FRandromGroup GroupData;
				GroupData.RandomData.Add(Item->ResultGrade, *Item);
				TableDataByGroupId.Add(Item->GroupId, GroupData);
			}

		}
	}
}
