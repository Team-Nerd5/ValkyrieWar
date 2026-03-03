// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseGameplayAbility.h"
#include "GA_BowAttack.generated.h"

class AArrowProjectile;

UCLASS()
class VALKYRIEWAR_API UGA_BowAttack : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BowAttack();

	//화살 발썃
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void FireArrow();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	// 화살칸
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AArrowProjectile> ProjectileClass;


	// 오토에임
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	float AutoAimRadius = 1500.0f;
	
};
