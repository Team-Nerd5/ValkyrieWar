// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ContentsModule.h"

void UContentsModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Contents);
		MakeData();
	}

	SendDataLoadComplete();
}

TArray<FContentsDataRow*> UContentsModule::GetAllDataSorted()
{
	TArray<FContentsDataRow*> ReturnArray;

	TableDataByDataId.GenerateValueArray(ReturnArray);
	//낮은 순서로 정렬..
	ReturnArray.Sort([](const FContentsDataRow& A, const FContentsDataRow& B) {
		return A.Order < B.Order;
		});

	return ReturnArray;
}

void UContentsModule::MakeData()
{
	if (DataTable)
	{
		TArray<FContentsDataRow*> AllRows;
		DataTable->GetAllRows<FContentsDataRow>(TEXT("ContentsModule_Init"), AllRows);

		for (FContentsDataRow* Item : AllRows)
		{
			TableDataByDataId.Add(Item->DataId, Item);
		}
	}
}

FContentsDataRow* const UContentsModule::GetTableData(int32 inDataId)
{
	return *TableDataByDataId.Find(inDataId);
}
