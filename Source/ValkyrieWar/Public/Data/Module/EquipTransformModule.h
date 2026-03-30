// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/EquipTransformDataRow.h"
#include "EquipTransformModule.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FOffsetGroupData
{
	GENERATED_BODY()
public:
	TMap<int32, FEquipTransformDataRow> DataByItemId;
};

UCLASS()
class VALKYRIEWAR_API UEquipTransformModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FOffsetGroupData GetDataListByValkyrieId(int32 InGroupId);

protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FOffsetGroupData> TableDataByGroupId;
};
