// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/UnitModule.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Enum/DataEnums.h"
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

bool UUnitModule::GetUnitDataRow(int32 InDataId, FUnitDataRow& OutRow) const
{
	const FUnitDataRow* Found = TableDataByDataId.Find(InDataId);
	if (!Found)
	{
		return false;
	}

	OutRow = *Found;
	return true;
}

bool UUnitModule::BuildComputedEnemyStat(int32 InDataId, int32 InLevel, FComputedEnemyStat& OutStat) const
{
	OutStat = FComputedEnemyStat{};

	if (OwnUnits.Contains(InDataId))
	{
		UUnitData* BaseData = OwnUnits.FindChecked(InDataId);

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

	return false;
}

bool UUnitModule::GetBonusStatByLevel(int32 InDataId, int32 InLevel, FStatValueData& OutBonusStat) const
{
	OutBonusStat = FStatValueData{};

	if (!GameManager.IsValid())
	{
		return false;
	}

	UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>();
	if (!DataManager)
	{
		return false;
	}

	if (OwnUnits.Contains(InDataId))
	{
		UUnitData* BaseData = OwnUnits.FindChecked(InDataId);
		if (!BaseData)
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

	return false;
}

void UUnitModule::LoadUnit(FUnitDataStruct InData)
{
	for (auto Data : TableDataByDataId)
	{
		if (Data.Value.UnitType == static_cast<EUnitCharacterType>(InData.UnitType) &&
			Data.Value.TeamType == ETeamType::Ally &&
			Data.Value.Grade == static_cast<EGradeType>(InData.Grade))
		{
			UUnitData* NewData = NewObject<UUnitData>(this);
			NewData->LoadData(InData.UID, Data.Value, InData.Level,  GameManager.Get());

			OwnUnits.Add(InData.UID, NewData);
		}
	}
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
		}
	}
	OwnUnits.Empty();
}

void UUnitModule::UnitLevelUpStat(int32 InDataId)
{
	if (OwnUnits.Contains(InDataId))
	{
		UUnitData* TargetUnitData = OwnUnits.FindChecked(InDataId);

		if (!TargetUnitData)
			return;

		UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>();
		if (!DataManager)
			return;

		// 유닛 레벨업
		TargetUnitData->LevelUp();

		FStatValueData BonusStatData = DataManager->GetUnitUpgradeStatModule()->GetTotalStat(TargetUnitData->GetLevelUpGroupId(), TargetUnitData->GetLevel());

		// 증가한 스텟 저장
		UnitAddedStats.Add(TargetUnitData->GetDataId(), BonusStatData);
	}
	
}
