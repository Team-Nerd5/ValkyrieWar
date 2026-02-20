// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseWall.h"
#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"

ABaseWall::ABaseWall()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseWall::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld())
    {
        if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
        {
            Subsys->RegisterWallAnchor(Team, this);
        }
    }
}

void ABaseWall::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
        {
            Subsys->UnregisterWallAnchor(Team, this);
        }
    }

    Super::EndPlay(EndPlayReason);
}
