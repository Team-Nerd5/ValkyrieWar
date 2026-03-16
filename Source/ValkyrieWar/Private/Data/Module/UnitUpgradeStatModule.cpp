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
		return  UpgradeData.FindRef(InGroupId).UpgradeDataList.FindChecked(InNextLevel);
	}
	return nullptr;
}

FStatValueData const UUnitUpgradeStatModule::GetTotalStat(int32 InGroupId, int32 InTargetLevel)
{
	//음...들어온 레벨까지의 모든 스탯 총합?
	return FStatValueData();
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
