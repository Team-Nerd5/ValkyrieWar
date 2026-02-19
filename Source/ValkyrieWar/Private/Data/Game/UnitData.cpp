// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/UnitData.h"
#include "Data/Table/GameData/ItemDataRow.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UUnitData::MakeData(const FUnitDataRow* InTableData, UGameManager* InGameManager)
{
	TableData = InTableData;

	if (InGameManager && InTableData)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			//기본 무기 데이터를 가져옴...
			FItemDataRow* BaseWeapon = DataManager->GetItemModule()->GetTableDataById(TableData->BaseWeaponId);
			if (BaseWeapon)
			{
				AttackData = DataManager->GetAttackModule()->GetAttackData(BaseWeapon->AttackId);
				SkillData = DataManager->GetSkillModule()->GetSkillData(BaseWeapon->SkillId);
			}
		}
	}
}
