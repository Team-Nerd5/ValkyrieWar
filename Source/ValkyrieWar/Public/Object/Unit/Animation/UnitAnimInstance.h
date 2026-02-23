// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/Enum/CommonEnums.h"
#include "UnitAnimInstance.generated.h"

class AUnitCharacter;
class UCharacterMovementComponent;

UCLASS()
class VALKYRIEWAR_API UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// ===== ABP/게임코드에서 호출 가능 =====
	UFUNCTION(BlueprintCallable, Category = "Unit|Anim")
	void ResetForReuse();

public:
	// ===== ABP에서 읽을 런타임 값들 =====
	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	TObjectPtr<AUnitCharacter> OwnerUnit = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	float Speed2D = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	float Direction = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bDead = false;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bInCombat = false;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bIsAttacking = false;

	// ===== 병과별 Locomotion BS =====
	// ABP에서 BlendSpace Player의 Asset으로 바로 꽂아 쓸 "현재 선택된" BS
	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim|Assets")
	TObjectPtr<UBlendSpace> LocomotionBS_Current = nullptr;

	// 어떤 병과에도 매핑이 없거나 병과를 못 읽었을 때 사용할 기본 BS
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Anim|Assets")
	TObjectPtr<UBlendSpace> LocomotionBS_Default = nullptr;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CachedMoveComp = nullptr;

	// Owner가 바뀌었는지 판단용 (풀링 재사용 시 중요)
	UPROPERTY()
	TWeakObjectPtr<APawn> CachedPawnOwner;

	void CacheOwner();
	void UpdateFromOwner(float DeltaSeconds);

	// 병과에 맞는 BS를 골라 LocomotionBS_Current에 넣는다
	void ApplyLocomotionBlendSpace();

	// 안전장치
	void EnsureLocomotionBSValid();
};
