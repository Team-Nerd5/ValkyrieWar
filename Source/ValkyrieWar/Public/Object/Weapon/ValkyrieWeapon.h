// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "ValkyrieWeapon.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AValkyrieWeapon : public ABaseActor
{
	GENERATED_BODY()
public:
	AValkyrieWeapon();

	virtual void ExecuteWeaponAction(AActor* InOwner, TSubclassOf<AActor> ProjectileClass) {}
	void SetWeaponMesh(class UItemData* InItem, class UAttackData* InAttackData);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
};
