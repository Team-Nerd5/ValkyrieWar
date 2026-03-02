// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/UnitDataRow.h"
#include "Data/Game/UnitData.h"
#include "UnitModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UUnitModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FORCEINLINE UUnitData* GetUnitDataById(int32 InDataId) const
	{
		return OwnUnits.FindRef(InDataId);
	}

protected:
	virtual void MakeData() override;
private:
	TMap<int32, FUnitDataRow> TableDataByDataId;

	//TODO: 임시 보유 유닛 데이터(전체 보유로 판단)
	UPROPERTY()
	TMap<int32, UUnitData*> OwnUnits;
};
