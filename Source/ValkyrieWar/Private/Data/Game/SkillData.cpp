// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/SkillData.h"
#include "GameSystem/Instance/Game/DataManager.h"

void USkillData::MakeData(const FSkillDataRow InTableData, UGameManager* InGameManager)
{
	TableData = InTableData;

	//SkillEffectModule에서 값 가져옴..

	if (InGameManager)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			EffectList = DataManager->GetSkillEffectModule()->GetEffects(TableData.EffectGroupId);
			if (TableData.ProjectileId > 0)
			{
				ProjectileData = DataManager->GetProjectileModule()->GetProjectile(TableData.ProjectileId);
			}

			TargetingData = DataManager->GetTargetingModule()->GetData(TableData.TargetingDataId);
		}
	}
}
