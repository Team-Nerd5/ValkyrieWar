// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "Data/Enum/CommonEnums.h"
#include "BaseWall.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ABaseWall : public ABaseActor
{
	GENERATED_BODY()

public:
    ABaseWall();

    UPROPERTY(EditAnywhere, Category = "Team")
    ETeam Team = ETeam::TeamA;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
