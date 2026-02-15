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

	FORCEINLINE USkillData* GetSkillData(int32 InDataId)
	{
		if (SkillData.IsEmpty() || !SkillData.Contains(InDataId))
			return nullptr;

		return *SkillData.Find(InDataId);
	}

protected:
	virtual void MakeData() override;

private:

	TMap<int32, USkillData*> SkillData;

};
