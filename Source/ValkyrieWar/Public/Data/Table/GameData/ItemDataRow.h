// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums.h"
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
	int32 DataId;
	UPROPERTY(EditAnywhere)
	EItemGroup ItemGroup;
	UPROPERTY(EditAnywhere)
	int32 StatId;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> Mesh = nullptr;
	UPROPERTY(EditAnywhere)
	int32 AttackId;
	UPROPERTY(EditAnywhere)
	int32 SkillId;
};
//상속 구조로 사용할까..? 장비랑 일반 아이템 분리
//없는값은 없는대로 사용할까..?
