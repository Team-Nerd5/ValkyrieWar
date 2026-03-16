// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ValkyrieModule.h"

void UValkyrieModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Valkyrie);
		MakeData();
	}
	SendDataLoadComplete();
}

bool UValkyrieModule::HasValkyrie(int32 InDataId)
{
	for (auto Valkyrie : OwnValkyries)
	{
		if (Valkyrie.Value->GetDataID() == InDataId)
		{
			return true;
		}
	}
	return false;
}

uint64 UValkyrieModule::CreateValkyrie(int32 InDataId)
{
	UValkyrieData* NewValkyrie = NewObject<UValkyrieData>(this);
	NewValkyrie->Initialize(*TableDataByDataId.Find(InDataId), GameManager.Get());

	OwnValkyries.Add(NewValkyrie->GetUID(), NewValkyrie);

	return NewValkyrie->GetUID();
}

void UValkyrieModule::LoadData(uint64 InUID, int32 InDataId)
{
	UValkyrieData* NewValkyrie = NewObject<UValkyrieData>(this);
	NewValkyrie->LoadData(InUID, *TableDataByDataId.Find(InDataId), GameManager.Get());

	OwnValkyries.Add(NewValkyrie->GetUID(), NewValkyrie);
}

TArray<int32> UValkyrieModule::GetAllByGrade(EGradeType InGradeType)
{
	TArray<int32> DataIdArray;

	for (auto Data : TableDataByDataId)
	{
		if (Data.Value.BaseGrade == InGradeType)
			DataIdArray.Add(Data.Value.DataId);
	}
	return DataIdArray;
}

void UValkyrieModule::MakeData()
{
	if (DataTable)
	{
		TArray<FValkyrieDataRow*> AllRows;
		DataTable->GetAllRows<FValkyrieDataRow>(TEXT("ValkyrieModule_Init"), AllRows);

		for (FValkyrieDataRow* Valkyrie : AllRows)
		{
			if (!Valkyrie) continue;

			TableDataByDataId.Add(Valkyrie->DataId, *Valkyrie);
		}
	}
}
