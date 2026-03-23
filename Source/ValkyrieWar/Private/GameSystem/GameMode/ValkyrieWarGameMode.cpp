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

void AValkyrieWarGameMode::RestartPlayer(AController* NewPlayer)
{
    if (UGameManager* GameManager = GetGameInstance<UGameManager>())
    {
        UValkyrieData* Selected = GameManager->GetSelectedValkyrie();

		if (Selected)
		{
			UClass* SpawnClass = Selected->GetSpawnClass().LoadSynchronous();
			if (SpawnClass)
			{
				AValkyrieCharacter* Valkyrie = SpawnValkyrie(NewPlayer, SpawnClass);
				if (Valkyrie)
				{
					Valkyrie->SetData(Selected);
				}
			}
		}
	}
}
AValkyrieCharacter* AValkyrieWarGameMode::SpawnValkyrie(AController* InController, TSubclassOf<APawn> PawnClassToSpawn)
{
    if (!IsValid(InController) || !IsValid(PawnClassToSpawn))
    {
        return nullptr;
    }

    if (APawn* OldPawn = InController->GetPawn())
    {
        InController->UnPossess();
        OldPawn->Destroy();
    }

    AActor* StartSpot = ChoosePlayerStart(InController);
    FTransform SpawnTransform = StartSpot ? StartSpot->GetActorTransform() : FTransform::Identity;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AValkyrieCharacter* SpawnedCharacter = GetWorld()->SpawnActor<AValkyrieCharacter>(PawnClassToSpawn, SpawnTransform, SpawnParams);

    if (IsValid(SpawnedCharacter))
    {
        InController->Possess(SpawnedCharacter);
    }

    return SpawnedCharacter;
}
