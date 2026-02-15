// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/SkillModule.h"

void USkillModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Skill);
		MakeData();
	}
}

void USkillModule::MakeData()
{
	if (DataTable)
	{
		TArray<FSkillDataRow*> AllRows;
		DataTable->GetAllRows<FSkillDataRow>(TEXT("SkillModule_Init"), AllRows);

		for (FSkillDataRow* Skill : AllRows)
		{
			USkillData* NewData = NewObject<USkillData>(this);
			NewData->MakeData(Skill, GameManager.Get());
			SkillData.Add(Skill->DataId, NewData);
		}
	}
}
