// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/UnitUpgradeStatModule.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UUnitUpgradeStatModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::UnitUpgradeStat);
		MakeData();
	}

	SendDataLoadComplete();
}

UUnitUpgradeData* const UUnitUpgradeStatModule::GetNextLevelData(int32 InGroupId, int32 InNextLevel)
{
	//예외처리 추가 확인 필요..
	if (UpgradeData.Contains(InGroupId))
	{
		if (FUnitStatLevelData* UnitUpgradeData = UpgradeData.Find(InGroupId))
		{
			if (UnitUpgradeData->UpgradeDataList.Contains(InNextLevel))
			{
				return UnitUpgradeData->UpgradeDataList.FindChecked(InNextLevel);
			}
		}
	}
	return nullptr;
}

FStatValueData const UUnitUpgradeStatModule::GetTotalStat(int32 InGroupId, int32 InTargetLevel)
{
	FStatValueData TotalStat;
	if (UpgradeData.Contains(InGroupId))
	{
		for (auto Data : UpgradeData.FindChecked(InGroupId).UpgradeDataList)
		{
			if (Data.Key > InTargetLevel)
				break;

			TotalStat.Add(Data.Value->GetAttack(), Data.Value->GetDefence(), Data.Value->GetHealth());
		}
	}

	return TotalStat;
}



void UUnitUpgradeStatModule::MakeData()
{
	if (DataTable)
	{
		TArray<FUnitUpgradeStatDataRow*> AllRows;
		DataTable->GetAllRows<FUnitUpgradeStatDataRow>(TEXT("UnitUpgradeStatModule_Init"), AllRows);

		if (GameManager.IsValid())
		{
			if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
			{
				for (FUnitUpgradeStatDataRow* Item : AllRows)
				{
					if (!Item) continue;

					UUnitUpgradeData* NewData = NewObject<UUnitUpgradeData>(this);
					NewData->MakeData(*Item, GameManager.Get());

					FUnitStatLevelData* FindData = &UpgradeData.FindOrAdd(Item->GroupId);
					FindData->UpgradeDataList.Add(Item->Level, NewData);
				}
			}			
		}			
	}
}
