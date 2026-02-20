// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/AttackModule.h"


void UAttackModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Attack);
		MakeData();
	}
	SendDataLoadComplete();
}

void UAttackModule::MakeData()
{
	if (DataTable)
	{
		TArray<FAttackDataRow*> AllRows;
		DataTable->GetAllRows<FAttackDataRow>(TEXT("AttackModule_Init"), AllRows);

		for (FAttackDataRow* Attack : AllRows)
		{
			UAttackData* NewData = NewObject<UAttackData>(this);
			NewData->MakeData(Attack, GameManager.Get());
			AttackData.Add(Attack->DataId, NewData);
		}
	}
}
