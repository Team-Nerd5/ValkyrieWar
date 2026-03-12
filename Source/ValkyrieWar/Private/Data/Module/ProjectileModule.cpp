// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ProjectileModule.h"

void UProjectileModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Projectile);
		MakeData();
	}

	SendDataLoadComplete();
}

void UProjectileModule::MakeData()
{
	if (DataTable)
	{
		TArray<FProjectileDataRow*> AllRows;
		DataTable->GetAllRows<FProjectileDataRow>(TEXT("ProjectileModule_Init"), AllRows);

		for (FProjectileDataRow* Projectile : AllRows)
		{
			if (!Projectile) continue;

			ProjectileData.Add(Projectile->DataId, *Projectile);
		}
	}
}
