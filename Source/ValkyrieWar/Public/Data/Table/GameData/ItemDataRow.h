// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "ItemDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FItemDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	EItemType ItemType;
	UPROPERTY(EditAnywhere)
	EItemGroup ItemGroup;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> Mesh = nullptr;
	UPROPERTY(EditAnywhere)
	int32 SellPrice = 0;

	//장비용
	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType==EItemType::Equip"))
	int32 StatId;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType==EItemType::Equip"))
	int32 AttackId;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType==EItemType::Equip"))
	TArray<int32> SkillId;
};
