// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "UnitCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AUnitCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
};
