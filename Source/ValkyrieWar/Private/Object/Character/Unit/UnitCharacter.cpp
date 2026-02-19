// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Unit/UnitCharacter.h"

void AUnitCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();    
}

void AUnitCharacter::SetData(UUnitData* InData)
{
	Data = InData;

	//기본 무기에 따른 공격/스킬 적용
	AttackData = InData->GetAttackData();

	SkillDataList = InData->GetSkillData();
}
