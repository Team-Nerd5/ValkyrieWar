// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Enum/CommonEnums.h"
#include "TestBaseWall.generated.h"

UCLASS()
class VALKYRIEWAR_API ATestBaseWall : public AActor
{
	GENERATED_BODY()
	
public:
    ATestBaseWall();

    UPROPERTY(EditAnywhere, Category = "Sample05")
    ETeam Team = ETeam::TeamA;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
