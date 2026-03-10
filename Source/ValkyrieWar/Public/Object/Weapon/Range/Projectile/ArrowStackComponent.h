// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArrowStackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VALKYRIEWAR_API UArrowStackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArrowStackComponent();

	UPROPERTY()
	TArray<class AArrowProjectile*> StackingArrows;

	void AddArrow(class AArrowProjectile* InArrow);

	void PullIt(float DamagePerArrow);

	void ClearAllArrows();
};
