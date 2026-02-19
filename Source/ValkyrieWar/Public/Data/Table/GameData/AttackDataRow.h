// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/DataEnums.h"
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
	TSoftObjectPtr<UAnimInstance> AnimInstance = nullptr;		//애니메이션 블루프린트
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimMontage> AnimMontage = nullptr;		//공격 몽타주
	UPROPERTY(EditAnywhere)
	int32 EffectGroupId = 0;							//GAS 등록용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere)
	FVector PositionOffset = FVector::ZeroVector;		//무기 포지션 오프셋
	UPROPERTY(EditAnywhere)
	FVector RotatationOffset = FVector::ZeroVector;		//벡터 말고 다른걸로 해야할 지 확인 필요
	UPROPERTY(EditAnywhere)
	ETargetType TargetType = ETargetType::None;
	UPROPERTY(EditAnywhere)
	int32 TargetAmount = 1;
};
