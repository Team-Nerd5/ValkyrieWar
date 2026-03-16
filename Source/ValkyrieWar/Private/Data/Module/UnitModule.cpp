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
	UUnitData* TargetUnitData = OwnUnits.FindRef(InDataId);
	if (!TargetUnitData)
		return;

	// 저장된 증가한 스텟 가져오기
	FStatValueData IncreaseStatData;
	IncreaseStatData = UnitAddedStats.FindRef(TargetUnitData->GetDataId());

	// 유닛 레벨업
	TargetUnitData->LevelUp();

	// 다음 레벨 데이터 가져오기
	UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>();
	int32 StatGroupId = DataManager->GetUnitUpgradeStatModule()->GetStatGroupId(TargetUnitData->GetLevel());
	FStatGroupDataRow BonusStatData = DataManager->GetUnitUpgradeStatModule()->GetStat(StatGroupId, TargetUnitData->GetLevel());

	IncreaseStatData.Attack += BonusStatData.Attack;
	IncreaseStatData.Health += BonusStatData.Health;
	IncreaseStatData.Defence += BonusStatData.Defence;

	UnitAddedStats.Add(TargetUnitData->GetDataId(), IncreaseStatData);
}
