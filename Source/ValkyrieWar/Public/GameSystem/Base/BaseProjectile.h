// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "AbilitySystemInterface.h"

#include "BaseProjectile.generated.h"

class UAbilitySystemComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ABaseProjectile : public ABaseActor, public IAbilitySystemInterface, public IObjectPoolInterface
{
	GENERATED_BODY()

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {
		return AbilitySystemComponent;
	}
public:

	ABaseProjectile();

	virtual void OnGet_Implementation() override;

	virtual void OnRelease_Implementation() override;

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> Effect = nullptr;

	UPROPERTY()
	TObjectPtr<UProjectileMovementComponent> MovementComponent = nullptr;
};
