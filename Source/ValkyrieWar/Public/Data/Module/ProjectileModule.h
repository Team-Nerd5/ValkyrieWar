// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/ProjectileDataRow.h"
#include "ProjectileModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UProjectileModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	FORCEINLINE FProjectileDataRow const GetProjectile(int32 InDataId)
	{
		if (ProjectileData.Contains(InDataId))
			return *ProjectileData.Find(InDataId);

		return FProjectileDataRow();
	}
protected:
	virtual void MakeData() override;

	UPROPERTY()
	TMap<int32, FProjectileDataRow> ProjectileData;
};
