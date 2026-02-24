// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/Enum/CharacterEnums.h"

#include "ValkyrieAnimInstance.generated.h"


class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UValkyrieAnimInstance();



protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;



	UPROPERTY()
	UCharacterMovementComponent* MovementComp;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<class AValkyrieCharacter> OwnerCharacter = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	EWeaponAnimType WeaponType = EWeaponAnimType::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float GroundSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsMoving = false;
};
