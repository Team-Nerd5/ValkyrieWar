// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FCharacterDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId;
	UPROPERTY(EditAnywhere)
	int32 BaseGrade;
	UPROPERTY(EditAnywhere)
	int32 StatId;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> Mesh = nullptr;
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100.0f;
	UPROPERTY(EditAnywhere)
	int32 BaseWeaponId;
	UPROPERTY(EditAnywhere)
	int32 BaseSkillId;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere)
	FString SocketName;

};
