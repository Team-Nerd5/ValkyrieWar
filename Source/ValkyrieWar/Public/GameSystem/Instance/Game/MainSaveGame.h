// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/PlayerSaveData.h"
#include "MainSaveGame.generated.h"

// 세이브 파일 저장경로 : ValkyrieWar/Saved/SaveGames 안에 있는 .sav 파일

UCLASS()
class VALKYRIEWAR_API UMainSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FPlayerSaveData PlayerData;

};
