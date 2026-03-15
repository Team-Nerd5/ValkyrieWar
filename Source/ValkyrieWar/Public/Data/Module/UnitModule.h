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

	FORCEINLINE void GetOwnedUnitIds(TArray<int32>& OutIds) const
	{
		OwnUnits.GenerateKeyArray(OutIds);
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

protected:
	virtual void MakeData() override;

public:
	void UnitLevelUpStat(int32 InUnitId);

private:
	TMap<int32, FUnitDataRow> TableDataByDataId;

	//TODO: 임시 보유 유닛 데이터(전체 보유로 판단)
	UPROPERTY()
	TMap<int32, TObjectPtr<UUnitData>> OwnUnits;

	UPROPERTY()
	TMap<int32, FStatValueData> UnitStats;

	// StatGroupId를 어떻게 써야할지 이해가 되지 않아서 임시로 사용
	int32 StatGroupId1 = 670001;
	int32 StatGroupId2 = 670002;
	int32 StatGroupId3 = 670003;
};
