// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/GachaRandomDataRow.h"
#include "GachaRandomModule.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FRandromGroup
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TMap<int32, FGachaRandomDataRow> RandomData;
};

UCLASS()
class VALKYRIEWAR_API UGachaRandomModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	TMap<int32, FGachaRandomDataRow> GetRandomData(int32 InGroupId);
protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FRandromGroup> TableDataByGroupId;
};
