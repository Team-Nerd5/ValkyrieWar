// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Table/GameData/AttackDataRow.h"
#include "Data/Game/SkillEffectData.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "AttackData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UAttackData : public UObject
{
	GENERATED_BODY()
public:
	void MakeData(const FAttackDataRow* InTableData, UGameManager* InGameManager);

	FORCEINLINE UAnimInstance* GetAnimInstance() { return TableData->AnimInstance.LoadSynchronous(); }
	FORCEINLINE UAnimMontage* GetAnimMontage() { return TableData->AnimMontage.LoadSynchronous(); }
	FORCEINLINE TArray<USkillEffectData*> GetEffectList() { return EffectList; }
private:
	const FAttackDataRow* TableData;

	TArray<USkillEffectData*> EffectList;
};
