// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/UnitDataRow.h"
#include "Data/Game/UnitData.h"
#include "Data/Struct/ComputedEnemyStat.h"
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

	FORCEINLINE void GetOwnedUnitIds(TArray<int32>& OutIds) const
	{
		OwnUnits.GenerateKeyArray(OutIds);
	}

	FORCEINLINE void GetUnitIdsByTeam(ETeamType InTeamType, TArray<int32>& OutIds) const
	{
		OutIds.Reset();
		for (const auto& TableData : TableDataByDataId)
		{
			const int32 DataId = TableData.Key;
			const FUnitDataRow& Row = TableData.Value;
			if (Row.TeamType == InTeamType)
			{
				OutIds.Add(DataId);
			}
		}
	}

	FORCEINLINE TSubclassOf<class AUnitCharacter> GetSpawnUnitClass(int32 InDataId)
	{
		return TableDataByDataId.FindRef(InDataId).SpawnClass;
	}

	FORCEINLINE ETeamType GetUnitTeam(int32 InDataId)
	{
		return TableDataByDataId.FindRef(InDataId).TeamType;
	}

	FORCEINLINE EPoolTypes GetUnitPoolType(int32 InDataId)
	{
		return TableDataByDataId.FindRef(InDataId).PoolType;
	}

	FORCEINLINE TSoftObjectPtr<UTexture2D> GetUnitIcon(int32 InDataId)
	{
		return TableDataByDataId.FindRef(InDataId).UnitIcon;
	}

	FORCEINLINE FStatValueData GetUnitStat(int32 InDataId)
	{
		return UnitAddedStats.FindRef(InDataId);
	}

	bool GetUnitDataRow(int32 InDataId, FUnitDataRow& OutRow) const;

	bool BuildComputedEnemyStat(int32 InDataId, int32 InLevel, FComputedEnemyStat& OutStat) const;
	bool GetBonusStatByLevel(int32 InDataId, int32 InLevel, FStatValueData& OutBonusStat) const;

protected:
	virtual void MakeData() override;

public:
	void UnitLevelUpStat(int32 InUnitId);

private:
	TMap<int32, FUnitDataRow> TableDataByDataId;

	//TODO: 임시 보유 유닛 데이터(전체 보유로 판단)
	UPROPERTY()
	TMap<int32, TObjectPtr<UUnitData>> OwnUnits;

	// 증가한 스텟 저장
	UPROPERTY()
	TMap<int32, FStatValueData> UnitAddedStats;
};
