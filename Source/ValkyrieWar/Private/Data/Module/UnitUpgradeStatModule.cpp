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

FStatGroupDataRow const UUnitUpgradeStatModule::GetStat(int32 InGroupId, int32 InLevel)
{
	if (StatData.Contains(InGroupId))
	{
		FUnitStatLevelData StatGroupData = StatData.FindChecked(InGroupId);

		if (StatGroupData.UnitStatData.Contains(InLevel))
		{
			return StatGroupData.UnitStatData.FindChecked(InLevel);
		}
	}
	//없으면 빈값 그냥 내보내도록...
	return FStatGroupDataRow();
}

//현재 레벨이니까... 테이블에서 현재 레벨 - 1까지 더해줌
FStatValueData const UUnitUpgradeStatModule::GetTotalStat(int32 InGroupId, int32 InTargetLevel)
{
	FStatValueData Data;

	if (StatData.Contains(InGroupId) && InTargetLevel > 1)
	{
		FUnitStatLevelData StatGroupData = StatData.FindChecked(InGroupId);

		for (int32 i = 2; i < InTargetLevel; i++)
		{
			FStatGroupDataRow LevelStat = StatGroupData.UnitStatData.FindChecked(i);
			Data.Attack += LevelStat.Attack;
			Data.Defence += LevelStat.Defence;
			Data.Health += LevelStat.Health;
		}
	}

	return Data;
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

					FStatGroupDataRow Stat = DataManager->GetStatGroupModule()->GetData(Item->StatGroupId);
					if (!StatData.Contains(Item->StatGroupId))
					{
						FUnitStatLevelData NewData;
						NewData.UnitStatData.Add(Item->Level, Stat);
						StatData.Add(Item->StatGroupId, NewData);
					}
					else
					{
						FUnitStatLevelData* FindData = &StatData.FindChecked(Item->StatGroupId);
						FindData->UnitStatData.Add(Item->Level, Stat);
					}
				}
			}			
		}			
	}
}
