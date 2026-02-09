// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestActors/TestBaseWall.h"
#include "Test/SDCH/TestGameSystem/TestInstance/TestWorld/TestBattleDirectorSubsystem.h"

ATestBaseWall::ATestBaseWall()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
}

void ATestBaseWall::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld())
    {
        if (UTestBattleDirectorSubsystem* Subsys = World->GetSubsystem<UTestBattleDirectorSubsystem>())
        {
            Subsys->RegisterWallAnchor(Team, this);
        }
    }
}

void ATestBaseWall::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UTestBattleDirectorSubsystem* Subsys = World->GetSubsystem<UTestBattleDirectorSubsystem>())
        {
            Subsys->UnregisterWallAnchor(Team, this);
        }
    }

    Super::EndPlay(EndPlayReason);
}
