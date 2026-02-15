// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/AttackData.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UAttackData::MakeData(const FAttackDataRow* InTableData, UGameManager* InGameManager)
{
	TableData = InTableData;

	//SkillEffectModule에서 값 가져옴..

	if (InGameManager && InTableData)
	{
		UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>();
		if (DataManager)
		{
			EffectList = DataManager->GetSkillEffectModule()->GetEffects(TableData->EffectGroupId);
		}
	}
	
}
