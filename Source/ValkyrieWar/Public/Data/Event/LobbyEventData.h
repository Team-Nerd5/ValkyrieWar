// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enum/StateEnums.h"
#include "LobbyEventData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyStateChanged, ELobbyState, InState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyCameraSet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadLobby);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadGacha, int32, InAmount, int32, InGachaGroupId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShowNextGacha);

/**
 * 
 */
USTRUCT(BlueprintType)
struct VALKYRIEWAR_API FLobbyEventData
{
	GENERATED_BODY()
public:
	FOnLobbyStateChanged OnLobbyStateChanged;
	FOnLobbyCameraSet OnLobbyCameraSet;
	FOnLoadLobby OnLoadLobby;
	FOnLoadGacha OnLoadGacha;
	FOnShowNextGacha OnShowNextGacha;
};
