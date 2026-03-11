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

	inline ETeamType GetTeam() const { return Team; }
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	ETeamType Team = ETeamType::Ally;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<class UStatAttributeSet> StatAttribute = nullptr;
};
