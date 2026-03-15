// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/UnitModule.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UUnitModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Unit);
		MakeData();
	}

	SendDataLoadComplete();
}

void UUnitModule::MakeData()
{
	if (DataTable)
	{
		TArray<FUnitDataRow*> AllRows;
		DataTable->GetAllRows<FUnitDataRow>(TEXT("UnitModule_Init"), AllRows);

		for (FUnitDataRow* Item : AllRows)
		{
			if (!Item) continue;

			TableDataByDataId.Add(Item->DataId, *Item);

			UUnitData* UnitData = NewObject<UUnitData>(this);
			UnitData->MakeData(*Item, GameManager.Get());

			OwnUnits.Add(Item->DataId, UnitData); 
		}
	}
}

void UUnitModule::UnitLevelUpStat(int32 InDataId)
{
	// UI구현을 위해 임시 사용
	FStatValueData IncreaseStatData;

	UUnitData* TargetUnitData = OwnUnits.FindRef(InDataId);
	if (!TargetUnitData)
		return;

	TargetUnitData->LevelUp();

	int32 StatGroupId = 0;
	int32 Temp = TargetUnitData->GetLevel() % 3;
	switch (Temp)
	{
	case 1:
		StatGroupId = StatGroupId1;
		break;
	case 2:
		StatGroupId = StatGroupId2;
		break;
	case 0:
		StatGroupId = StatGroupId3;
		break;
	default:
		break;
	}


	if (UnitStats.Contains(TargetUnitData->GetDataId()))
	{
		IncreaseStatData = UnitStats.FindRef(TargetUnitData->GetDataId());
	}

	UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>();
	FStatGroupDataRow BonusStatData = DataManager->GetUnitUpgradeStatModule()->GetStat(StatGroupId, TargetUnitData->GetLevel());

	IncreaseStatData.Attack += BonusStatData.Attack;
	IncreaseStatData.Health += BonusStatData.Health;
	IncreaseStatData.Defence += BonusStatData.Defence;

	UnitStats.Add(TargetUnitData->GetDataId(), IncreaseStatData);
}
