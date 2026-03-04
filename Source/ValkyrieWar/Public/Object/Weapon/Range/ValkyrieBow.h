// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Weapon/ValkyrieWeapon.h"
#include "ValkyrieBow.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AValkyrieBow : public AValkyrieWeapon
{
	GENERATED_BODY()
public:
	virtual void ExecuteWeaponAction(AActor* InOwner, TSubclassOf<AActor> ProjectileClass) override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AActor> ArrowClass;
	
};
