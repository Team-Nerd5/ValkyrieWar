// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Data/Table/GameData/ProjectileDataRow.h"
#include "Data/Struct/GameplayCueData.h"

#include "BaseProjectile.generated.h"

class UAbilitySystemComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;

/**
 * 
 */
UCLASS(BlueprintType)
class VALKYRIEWAR_API ABaseProjectile : public ABaseActor, public IAbilitySystemInterface, public IObjectPoolInterface
{
	GENERATED_BODY()

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {
		return AbilitySystemComponent;
	}
public:

	ABaseProjectile();

	virtual void BeginPlay() override;

	virtual void OnGet_Implementation() override;

	virtual void OnRelease_Implementation() override;

	void SetData(FGameplayTag InTag, FGameplayAbilitySpec InSpec, FProjectileDataRow InProjectileData, TArray<FGameplayCueData> InCues);

	void SetAttack(float InAttack);

	void SetTeam(ETeamType InTeam) { TeamType = InTeam; }

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStatAttributeSet> StatAttributeSet;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> Effect = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> MovementComponent = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> Collision = nullptr;

	FGameplayTag AbilityTag;

	EPoolTypes PoolType = EPoolTypes::None;

	TArray<FGameplayCueData> PlayCueOnTarget;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	ETeamType TeamType = ETeamType::None;
};
