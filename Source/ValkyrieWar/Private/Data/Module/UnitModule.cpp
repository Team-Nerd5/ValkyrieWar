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

bool UUnitModule::BuildComputedEnemyStat(int32 InDataId, int32 InLevel, FComputedEnemyStat& OutStat) const
{
	OutStat = FComputedEnemyStat{};

	UUnitData* BaseData = OwnUnits.FindRef(InDataId);
	if (!BaseData)
	{
		return false;
	}

	FStatValueData BonusStat;
	GetBonusStatByLevel(InDataId, InLevel, BonusStat);

	OutStat.DataId = InDataId;
	OutStat.Level = FMath::Max(1, InLevel);
	OutStat.Attack = BaseData->GetStat(EStatusType::Attack) + BonusStat.Attack;
	OutStat.Defence = BaseData->GetStat(EStatusType::Defence) + BonusStat.Defence;
	OutStat.Health = BaseData->GetStat(EStatusType::Health) + BonusStat.Health;

	return true;
}

bool UUnitModule::GetBonusStatByLevel(int32 InDataId, int32 InLevel, FStatValueData& OutBonusStat) const
{
	OutBonusStat = FStatValueData{};

	UUnitData* BaseData = OwnUnits.FindRef(InDataId);
	if (!BaseData)
	{
		return false;
	}

	if (!GameManager.IsValid())
	{
		return false;
	}

	UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>();
	if (!DataManager)
	{
		return false;
	}

	UUnitUpgradeStatModule* UpgradeModule = DataManager->GetUnitUpgradeStatModule();
	if (!UpgradeModule)
	{
		return false;
	}

	OutBonusStat = UpgradeModule->GetTotalStat(BaseData->GetLevelUpGroupId(), FMath::Max(1, InLevel));
	return true;
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

	// 유닛 레벨업
	TargetUnitData->LevelUp();

	UUnitUpgradeData* UpgradeData = DataManager->GetUnitUpgradeStatModule()->GetNextLevelData(TargetUnitData->GetLevelUpGroupId(), TargetUnitData->GetLevel());

	//UnitData의 테이블 데이터를 코드로 바꿔버리는데 이러면 큰일납니다.
	//데이터 자체를 바꿔버리는거에요. 게임 돌때마다 테이블이 바뀌는 일이 발생합니다.

	FStatValueData BonusStatData = DataManager->GetUnitUpgradeStatModule()->GetTotalStat(TargetUnitData->GetLevelUpGroupId(), TargetUnitData->GetLevel());

	// 증가한 스텟 저장
	UnitAddedStats.Add(TargetUnitData->GetDataId(), BonusStatData);
}
