// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/UnitData.h"
#include "Data/Table/GameData/ItemDataRow.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UUnitData::MakeData(const FUnitDataRow InTableData, UGameManager* InGameManager)
{
	TableData = InTableData;
	Stat.Empty();

	if (InGameManager)
	{
		UID = InGameManager->GetItemUID();

		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			//기본 무기 데이터를 가져옴...
			FItemDataRow BaseWeapon = DataManager->GetItemModule()->GetTableDataById(TableData.BaseWeaponId);
			if (BaseWeapon.DataId > 0)
			{
				AttackData = DataManager->GetAttackModule()->GetAttackData(BaseWeapon.AttackId);
				SkillData = DataManager->GetSkillModule()->GetSkillData(BaseWeapon.SkillId);
			}

			FStatGroupDataRow StatData = DataManager->GetStatGroupModule()->GetData(TableData.StatId);
			if (StatData.DataId > 0)
			{
				Stat.Add(EStatusType::Attack, StatData.Attack);
				Stat.Add(EStatusType::Defence, StatData.Defence);
				Stat.Add(EStatusType::Health, StatData.Health);
			}
		}
	}
}

void UUnitData::LoadData(uint64 InUID, const FUnitDataRow InTableData, int32 InLevel, UGameManager* InGameManager)
{
	UID = InUID;
	TableData = InTableData;
	Level = InLevel;
	Stat.Empty();

	if (InGameManager)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			//기본 무기 데이터를 가져옴...
			FItemDataRow BaseWeapon = DataManager->GetItemModule()->GetTableDataById(TableData.BaseWeaponId);
			if (BaseWeapon.DataId > 0)
			{
				AttackData = DataManager->GetAttackModule()->GetAttackData(BaseWeapon.AttackId);
				SkillData = DataManager->GetSkillModule()->GetSkillData(BaseWeapon.SkillId);
			}

			FStatGroupDataRow StatData = DataManager->GetStatGroupModule()->GetData(TableData.StatId);
			if (StatData.DataId > 0)
			{
				Stat.Add(EStatusType::Attack, StatData.Attack);
				Stat.Add(EStatusType::Defence, StatData.Defence);
				Stat.Add(EStatusType::Health, StatData.Health);
			}
		}
	}
}

void UUnitData::LevelUp()
{
	Level++;
}
