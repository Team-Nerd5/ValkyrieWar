// TestUnitAnimInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TestUnitAnimInstance.generated.h"

class ATestBaseUnit;
class UCharacterMovementComponent;

UCLASS(Blueprintable, BlueprintType)
class VALKYRIEWAR_API UTestUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTestUnitAnimInstance();

	// ===== ABP에서 읽을 런타임 값들 =====
	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	TObjectPtr<ATestBaseUnit> OwnerUnit = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	float Speed2D = 0.f;

	// 이동 방향(-180~180). 스트레이프/8방향 블렌드에 유용
	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	float Direction = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bDead = false;

	// “전투 중인지” (ReservedTarget 존재 등)
	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bInCombat = false;

	// 옵션: 공격 중(몽타주 재생중 등) 표시
	UPROPERTY(BlueprintReadOnly, Category = "Unit|Anim")
	bool bIsAttacking = false;

	// ===== ABP/게임코드에서 호출 가능 =====
	// 풀에서 재사용(OnGet) 시 애니 상태를 깨끗하게 초기화
	UFUNCTION(BlueprintCallable, Category = "Unit|Anim")
	void ResetForReuse();

	// 공격 몽타주 재생(원하면)
	UFUNCTION(BlueprintCallable, Category = "Unit|Anim")
	bool PlayAttackMontage(UAnimMontage* Montage, float PlayRate = 1.f, FName StartSection = NAME_None);

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	TObjectPtr<UCharacterMovementComponent> CachedMoveComp = nullptr;

	void CacheOwner();
	void UpdateFromOwner(float DeltaSeconds);
};
