// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void SetInstacne(UBlendSpace* InBlendSpace, ABaseCharacter* InOwner);
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void InitAnimData();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UBlendSpace> DefaultBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> LocomotionBS = nullptr;

	UPROPERTY(BlueprintReadOnly)
	float MoveSpeed = 0.0f;

	UPROPERTY()
	TObjectPtr<ABaseCharacter> Owner = nullptr;


};
