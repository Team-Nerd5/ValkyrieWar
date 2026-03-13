// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/ValkyrieData.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UValkyrieData::MakeData(const FValkyrieDataRow InTableData, UGameManager* InGameManager)
{
	TableData = InTableData;
	Stat.Empty();

	if (InGameManager)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			FItemDataRow BaseData = DataManager->GetItemModule()->GetTableDataById(TableData.BaseWeaponId);
			if (BaseData.DataId > 0)
			{
				BaseWeapon = NewObject<UItemData>(this);
				BaseWeapon->MakeData(BaseData);

				AttackData = DataManager->GetAttackModule()->GetAttackData(BaseData.AttackId);
				SkillData = DataManager->GetSkillModule()->GetSkillData(BaseData.SkillId);
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
//장비를 장착하거나 아이템 로드 시 장착된 캐릭터에 장착 처리에 사용
void UValkyrieData::EquipItem(EEquipGroup InEquipGroup, UItemData* InItem, UGameManager* InGameManager)
{
	if (!InGameManager)
		return;

	if (InItem)
	{
		//있으면 덮어쓰기
		EquippedItem.Add(InEquipGroup, InItem);
	}

	if (InEquipGroup == EEquipGroup::Weapon)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			AttackData = DataManager->GetAttackModule()->GetAttackData(InItem->GetAttackID());
			SkillData = DataManager->GetSkillModule()->GetSkillData(InItem->GetSkillID());
		}
	}

	//TODO : 장비 스탯 업데이트
}

//장비 해제 처리
void UValkyrieData::UnEquipItem(EEquipGroup InEquipGroup, UGameManager* InGameManager)
{
	if (InEquipGroup == EEquipGroup::Weapon)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			AttackData = DataManager->GetAttackModule()->GetAttackData(BaseWeapon->GetAttackID());
			SkillData = DataManager->GetSkillModule()->GetSkillData(BaseWeapon->GetSkillID());
		}
	}

	UItemData* Equipping = EquippedItem.FindChecked(InEquipGroup);
	Equipping->Equip(0);

	EquippedItem.Remove(InEquipGroup);

	//TODO : 장비 스탯 업데이트
}

void UValkyrieData::Initialize(const FValkyrieDataRow InTableData, UGameManager* InGameManager)
{
	 UID = InGameManager->GetValkyrieUID();

	 MakeData(InTableData, InGameManager);
}

void UValkyrieData::LoadData(uint64 InUID, const FValkyrieDataRow InTableData, UGameManager* InGameManager)
{
	UID = InUID;
	MakeData(InTableData, InGameManager);
}
