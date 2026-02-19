// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Game/ValkyrieData.h"
#include "Data/Table/GameData/ValkyrieDataRow.h"
#include "ValkyrieModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FORCEINLINE UValkyrieData* GetExistValkyrie(uint64 InUID)
	{
		if (OwnValkyries.Contains(InUID))
			return *OwnValkyries.Find(InUID);
		else
			return nullptr;
	}
protected:
	virtual void MakeData() override;
private:
	FValkyrieDataRow* GetTableDataById(int32 InDataId);

private:
	TMap<uint64, UValkyrieData*> OwnValkyries;

	TMap<int32, FValkyrieDataRow*> TableDataByDataId;
};
