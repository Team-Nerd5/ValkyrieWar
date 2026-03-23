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

	FORCEINLINE UValkyrieData* GetFirstValkyrie()
	{
		if (OwnValkyries.Num() > 0)
		{
			return OwnValkyries.begin().Value();
		}
		else
			return nullptr;
	}
	FORCEINLINE FValkyrieDataRow GetTableData(int32 InDataId)
	{
		if (TableDataByDataId.Contains(InDataId))
			return TableDataByDataId.FindRef(InDataId);
		else
			return FValkyrieDataRow();
	}

	bool HasValkyrie(int32 InDataId);

	UValkyrieData* CreateValkyrie(int32 InDataId);

	void LoadData(uint64 InUID, int32 InDataId);

	TArray<int32> GetAllByGrade(EGradeType InGradeType);
protected:
	virtual void MakeData() override;
private:
	FValkyrieDataRow* GetTableDataById(int32 InDataId);

private:
	UPROPERTY()
	TMap<uint64, TObjectPtr<UValkyrieData>> OwnValkyries;

	UPROPERTY()
	TMap<int32, FValkyrieDataRow> TableDataByDataId;
};
