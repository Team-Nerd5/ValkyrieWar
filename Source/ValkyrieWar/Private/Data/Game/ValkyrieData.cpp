// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/ValkyrieData.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UValkyrieData::MakeData(const FValkyrieDataRow* InTableData, UGameManager* InGameManager)
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

void UValkyrieData::UpdateWeapon(UItemData* InNewWeapon, UGameManager* InGameManager)
{
	if (InGameManager && InNewWeapon)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			AttackData = DataManager->GetAttackModule()->GetAttackData(InNewWeapon->GetAttackID());
			SkillData = DataManager->GetSkillModule()->GetSkillData(InNewWeapon->GetSkillID());
		}
	}
}

void UValkyrieData::Initialize(const FValkyrieDataRow* InTableData, UGameManager* InGameManager)
{
	 UID = InGameManager->GetCharacterUID();

	 MakeData(InTableData, InGameManager);
}

void UValkyrieData::LoadData(uint64 InUID, const FValkyrieDataRow* InTableData, UGameManager* InGameManager)
{
	UID = InUID;
	MakeData(InTableData, InGameManager);
}
