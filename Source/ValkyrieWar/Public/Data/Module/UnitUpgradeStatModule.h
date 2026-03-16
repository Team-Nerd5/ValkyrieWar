// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Game/UnitUpgradeData.h"
#include "Data/Struct/StatValueData.h"
#include "UnitUpgradeStatModule.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FUnitStatLevelData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TMap<int32, TObjectPtr<UUnitUpgradeData>> UpgradeDataList;	//레벨에 따른 데이터
};

UCLASS()
class VALKYRIEWAR_API UUnitUpgradeStatModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	//다음 레벨의 데이터를 가져옴(다음 레벨이 되기위한 비용, 다음 레벨에 오르는 스탯)
	UUnitUpgradeData* const GetNextLevelData(int32 InGroupId, int32 InNextLevel);

	FStatValueData const GetTotalStat(int32 InGroupId, int32 InTargetLevel);

protected:
	virtual void MakeData() override;
private:
	UPROPERTY()
	TMap<int32, FUnitStatLevelData> UpgradeData;
};
