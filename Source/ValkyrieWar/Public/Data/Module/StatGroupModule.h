// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/StatGroupDataRow.h"
#include "StatGroupModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UStatGroupModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FORCEINLINE FStatGroupDataRow* const GetData(int32 InDataId)
	{
		if (TableDataByDataId.Contains(InDataId))
		{
			return *TableDataByDataId.Find(InDataId);
		}
		return nullptr;
	}
protected:
	virtual void MakeData() override;
private:
	TMap<int32, FStatGroupDataRow*> TableDataByDataId;
};
