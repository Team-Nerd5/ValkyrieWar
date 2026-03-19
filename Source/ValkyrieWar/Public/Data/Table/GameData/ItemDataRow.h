// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "Data/Enum/CharacterEnums.h"
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
	EItemType ItemType = EItemType::None;
	UPROPERTY(EditAnywhere)
	FString Name = TEXT("");
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere)
	int32 SellPrice = 0;

	//장비용
	UPROPERTY(EditAnywhere)
	EEquipType EquipType;
	UPROPERTY(EditAnywhere)
	int32 StatId = 0;
	UPROPERTY(EditAnywhere)
	int32 AttackId = 0;
	UPROPERTY(EditAnywhere)
	TArray<int32> SkillId;
	UPROPERTY(EditAnywhere)
	bool IsSkeletal = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "IsSkeletal==false"))
	TSoftObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "IsSkeletal==true"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh = nullptr;
	UPROPERTY(EditAnywhere)
	FName SocketName = TEXT("");
	UPROPERTY(EditAnywhere)
	int32 SetGroupId = 0;
};
