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
            if (auto SpawnDataClass = Selected->GetSpawnClass())
            {
                UClass* SpawnClass = SpawnDataClass.LoadSynchronous();
                AValkyrieCharacter* Valkyrie = SpawnValkyrie(NewPlayer, SpawnClass);
                if (Valkyrie)
                {
                    Valkyrie->SetData(Selected);
                }
            }
        }
    }
}

AValkyrieCharacter* AValkyrieWarGameMode::SpawnValkyrie(AController* NewPlayer, TSubclassOf<APawn> PawnClassToSpawn)
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

    AActor* StartSpot = ChoosePlayerStart(NewPlayer); // 기존에 작성하신 함수 그대로 사용
    FTransform SpawnTransform = StartSpot ? StartSpot->GetActorTransform() : FTransform::Identity;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AValkyrieCharacter* SpawnedCharacter = GetWorld()->SpawnActor<AValkyrieCharacter>(PawnClassToSpawn, SpawnTransform, SpawnParams);

    if (IsValid(SpawnedCharacter))
    {
        // 여기서 빙의를 시켜주면, 엔진이 알아서 이 캐릭터를 뷰 타겟(View Target)으로 완벽하게 고정합니다.
        NewPlayer->Possess(SpawnedCharacter);
    }

    return SpawnedCharacter;
}
