// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "AbilitySystemInterface.h"
#include "Data/Enum/CommonEnums.h"
#include "CoreWallActor.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ACoreWallActor : public ABaseActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACoreWallActor();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	inline ETeam GetTeam() const { return Team; }
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	ETeam Team = ETeam::TeamA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<class UStatAttributeSet> StatAttribute = nullptr;
};
