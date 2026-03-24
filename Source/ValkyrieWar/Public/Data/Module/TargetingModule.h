// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/TargetingDataRow.h"
#include "TargetingModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTargetingModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FORCEINLINE FTargetingDataRow const GetData(int32 InDataId)
	{
		if (TargetingData.Contains(InDataId))
			return TargetingData.FindChecked(InDataId);
		else
			return FTargetingDataRow();
	}
protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FTargetingDataRow> TargetingData;
};
