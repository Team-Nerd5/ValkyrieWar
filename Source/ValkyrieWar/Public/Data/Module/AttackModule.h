// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/AttackDataRow.h"
#include "Data/Game/AttackData.h"
#include "AttackModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UAttackModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FORCEINLINE UAttackData* GetAttackData(int32 InDataId)
	{
		if (AttackData.IsEmpty() || !AttackData.Contains(InDataId))
			return nullptr;

		return *AttackData.Find(InDataId);
	}

protected:
	virtual void MakeData() override;
	
private:

	TMap<int32, UAttackData*> AttackData;
};
