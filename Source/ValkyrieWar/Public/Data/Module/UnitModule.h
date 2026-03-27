// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/UnitDataRow.h"
#include "Data/Game/UnitData.h"
#include "Data/Struct/ComputedEnemyStat.h"
#include "Data/Enum/DataEnums.h"
#include "Object/SaveGame/UnitUpgradeSaveGame.h"
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
		if (OwnUnits.Contains(InDataId))
			return OwnUnits.FindChecked(InDataId);
		else
			return nullptr;
	}

	FORCEINLINE void GetOwnedUnitIds(TArray<int32>& OutIds) const
	{
		OwnUnits.GenerateKeyArray(OutIds);
	}

	FORCEINLINE const TMap<int32, TObjectPtr<UUnitData>>& GetOwnedUnits() const
	{
		return OwnUnits;
	}

	FORCEINLINE TSubclassOf<class AUnitCharacter> GetSpawnUnitClass(int32 InDataId)
	{
		if (TableDataByDataId.Contains(InDataId))
			return TableDataByDataId.FindChecked(InDataId).SpawnClass;
		else
			return nullptr;
	}

	FORCEINLINE ETeamType GetUnitTeam(int32 InDataId)
	{
		if (TableDataByDataId.Contains(InDataId))
			return TableDataByDataId.FindChecked(InDataId).TeamType;
		else
			return ETeamType::None;
	}

	FORCEINLINE EPoolTypes GetUnitPoolType(int32 InDataId)
	{
		if (TableDataByDataId.Contains(InDataId))
			return TableDataByDataId.FindChecked(InDataId).PoolType;
		else
			return EPoolTypes::None;
	}

	FORCEINLINE TSoftObjectPtr<UTexture2D> GetUnitIcon(int32 InDataId)
	{
		if (TableDataByDataId.Contains(InDataId))
			return TableDataByDataId.FindChecked(InDataId).UnitIcon;
		else
			return nullptr;
	}

	FORCEINLINE FStatValueData GetUnitStat(int32 InDataId)
	{
		if (UnitAddedStats.Contains(InDataId))
			return UnitAddedStats.FindChecked(InDataId);
		else
			return FStatValueData();
	}

	bool GetUnitDataRow(int32 InDataId, FUnitDataRow& OutRow) const;

	bool BuildComputedEnemyStat(int32 InDataId, int32 InLevel, FComputedEnemyStat& OutStat) const;
	bool GetBonusStatByLevel(int32 InDataId, int32 InLevel, FStatValueData& OutBonusStat) const;

	void LoadUnit(FUnitDataStruct InData);
protected:
	virtual void MakeData() override;

public:
	void UnitLevelUpStat(int32 InDataId);

	UUnitData* CreateEnemyData(int32 InDataId);

private:
	TMap<int32, FUnitDataRow> TableDataByDataId;

	//TODO: 임시 보유 유닛 데이터(전체 보유로 판단)
	UPROPERTY()
	TMap<int32, TObjectPtr<UUnitData>> OwnUnits;

	// 증가한 스텟 저장
	UPROPERTY()
	TMap<int32, FStatValueData> UnitAddedStats;
};
