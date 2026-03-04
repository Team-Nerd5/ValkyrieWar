// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Game/SkillData.h"
#include "SkillModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API USkillModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	TArray<USkillData*> GetSkillData(TArray<int32> InDataId);

protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, TObjectPtr<USkillData>> SkillData;

};
