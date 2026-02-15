// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/SkillEffectModule.h"
#include "Data/Table/GameData/SkillEffectDataRow.h"

void USkillEffectModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::SkillEffect);
		MakeData();
	}
}

TArray<USkillEffectData*> USkillEffectModule::GetEffects(int32 InGroupId)
{
	if (EffectsByGroupId.Contains(InGroupId))
		return *EffectsByGroupId.Find(InGroupId);
	else
		return TArray<USkillEffectData*>();
}

void USkillEffectModule::MakeData()
{
	if (DataTable)
	{
		TArray<FSkillEffectDataRow*> AllRows;
		DataTable->GetAllRows<FSkillEffectDataRow>(TEXT("SkillEffectModule_Init"), AllRows);

		for (FSkillEffectDataRow* SkillEffect : AllRows)
		{
			USkillEffectData* Data = NewObject<USkillEffectData>(this);
			Data->MakeData(SkillEffect);

			EffectsByGroupId.FindOrAdd(Data->GetGroupId()).Add(Data);
		}
	}
}
