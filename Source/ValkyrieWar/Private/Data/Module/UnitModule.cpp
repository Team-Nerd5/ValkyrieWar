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
	UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;

	// 저장된 증가한 스텟 가져오기
	FStatValueData IncreaseStatData = UnitAddedStats.FindRef(TargetUnitData->GetDataId());

	// 유닛 레벨업
	TargetUnitData->LevelUp();
	
	// 다음 레벨 데이터ID 가져오기
	int32 StatGroupId = DataManager->GetUnitUpgradeStatModule()->GetStatGroupId(TargetUnitData->GetLevel());

	// 레벨에 해당되는 StatGroupId을 Unit에 저장
	TargetUnitData->SetLevelUpGroupId(StatGroupId);

	// 레벨업 이후 증가한 스텟 가져오기
	auto BonusStatData = DataManager->GetUnitUpgradeStatModule()->GetTotalStat(TargetUnitData->GetLevelUpGroupId(), TargetUnitData->GetLevel());

	if(BonusStatData.Attack != 0)
		IncreaseStatData.Attack = BonusStatData.Attack;
	if (BonusStatData.Health != 0)
	IncreaseStatData.Health = BonusStatData.Health;
	if (BonusStatData.Defence != 0)
	IncreaseStatData.Defence = BonusStatData.Defence;

	// 증가한 스텟 저장
	UnitAddedStats.Add(TargetUnitData->GetDataId(), IncreaseStatData);
}
