// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "Data/Enum/CharacterEnums.h"
#include "GameplayTagContainer.h"
#include "SkillDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FSkillDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimMontage> Montage = nullptr;
	UPROPERTY(EditAnywhere)
	int32 EffectGroupId = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere)
	ETargetType TargetType = ETargetType::None;
	UPROPERTY(EditAnywhere)
	int32 TargetAmount = 1;
	UPROPERTY(EditAnywhere)
	EAttackType AttackType = EAttackType::Melee;
	UPROPERTY(EditAnywhere)
	int32 ProjectileId = 0;
};
