// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameSystem/GameMode/ValkyrieWarGameMode.h"

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

#include "EngineUtils.h"

#include "GameFramework/PlayerStart.h"

AValkyrieWarGameMode::AValkyrieWarGameMode()
{
	DefaultPawnClass = nullptr;
}

AActor* AValkyrieWarGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (TActorIterator<APlayerStart> It(World); It; ++It)
        {
            APlayerStart* PlayerStart = *It;
            if (IsValid(PlayerStart))
            {
                return PlayerStart;
            }
        }
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}

AValkyrieCharacter* AValkyrieWarGameMode::SpawnValkyrie(APlayerController* NewPlayer, TSubclassOf<APawn> PawnClassToSpawn)
{
    if (!IsValid(NewPlayer) || !IsValid(PawnClassToSpawn))
    {
        return nullptr;
    }

    if (APawn* OldPawn = NewPlayer->GetPawn())
    {
        NewPlayer->UnPossess();
        OldPawn->Destroy();
    }

    AActor* StartSpot = ChoosePlayerStart_Implementation(NewPlayer);
    FTransform SpawnTransform = StartSpot ? StartSpot->GetActorTransform() : FTransform::Identity;

    //생성위치 충돌 시 조정
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AValkyrieCharacter* SpawnedCharacter = GetWorld()->SpawnActor<AValkyrieCharacter>(PawnClassToSpawn, SpawnTransform, SpawnParams);

    if (IsValid(SpawnedCharacter))
    {
        NewPlayer->Possess(SpawnedCharacter);
    }

    return SpawnedCharacter;
}
