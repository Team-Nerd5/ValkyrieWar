// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	int32 DataId;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimInstance> AnimInstance = nullptr;		//애니메이션 블루프린트
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimMontage> AnimMontage = nullptr;		//공격 몽타주
	UPROPERTY(EditAnywhere)
	int32 EffectGroupId;							//GAS 등록용
	UPROPERTY(EditAnywhere)
	FVector PositionOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FVector RotatationOffset = FVector::ZeroVector;		//벡터 말고 다른걸로 해야할 지 확인 필요

};
