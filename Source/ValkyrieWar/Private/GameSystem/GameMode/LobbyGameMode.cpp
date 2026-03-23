// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/GameMode/LobbyGameMode.h"
#include "GameSystem/State/Game/LobbyGameState.h"

#include "Data/Game/ValkyrieData.h"

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

#include "EngineUtils.h"

#include "GameFramework/PlayerStart.h"

ALobbyGameMode::ALobbyGameMode()
{
	DefaultPawnClass = nullptr;
}

AActor* ALobbyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (TActorIterator<APlayerStart> It(World); It; ++It)
        {
            APlayerStart* PlayerStart = *It;
            if (IsValid(PlayerStart) && PlayerStart->PlayerStartTag == PlayerStartTag)
            {
                return PlayerStart;
            }
        }
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}

AValkyrieCharacter* ALobbyGameMode::SpawnValkyire(UValkyrieData* InData, AController* InController, FName InTag)
{
    if (!InData)
        return nullptr;

    if (APawn* OldPawn = InController->GetPawn())
    {
        InController->UnPossess();
        OldPawn->Destroy();
    }

    PlayerStartTag = InTag;

    AActor* StartSpot = ChoosePlayerStart(InController);
    FTransform SpawnTransform = StartSpot ? StartSpot->GetActorTransform() : FTransform::Identity;

    UClass* SpawnClass = InData->GetSpawnClass().LoadSynchronous();

    AValkyrieCharacter* SpawnedCharacter = GetWorld()->SpawnActor<AValkyrieCharacter>(SpawnClass, SpawnTransform);

    if (IsValid(SpawnedCharacter))
    {
        InController->Possess(SpawnedCharacter);
    }
    return SpawnedCharacter;
}


