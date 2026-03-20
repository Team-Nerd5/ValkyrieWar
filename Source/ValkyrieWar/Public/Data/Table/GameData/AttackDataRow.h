// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Struct/GameplayCueData.h"
#include "GameplayTagContainer.h"
#include "AttackDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FAttackDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 DataId = 0;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimMontage> AnimMontage = nullptr;		//공격 몽타주
	UPROPERTY(EditAnywhere)
	int32 EffectGroupId = 0;							//GAS 등록용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere)
	FVector PositionOffset = FVector::ZeroVector;		//무기 포지션 오프셋
	UPROPERTY(EditAnywhere)
	FRotator RotateOffset = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere)
	EAttackType AttackType = EAttackType::Melee;
	UPROPERTY(EditAnywhere)
	int32 ProjectileId = 0;
	UPROPERTY(EditAnywhere)
	TArray<FGameplayCueData> CueData;
	UPROPERTY(EditAnywhere)
	float AttackRange = 100.0f;
	UPROPERTY(EditAnywhere)
	int32 TargetingDataId = 0;
};
