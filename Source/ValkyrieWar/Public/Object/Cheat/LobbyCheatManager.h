// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Data/Enum/DataEnums.h"
#include "LobbyCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULobbyCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(exec)
	void AddItem(int32 InItemID, int32 InAmount = 1);

	UFUNCTION(exec)
	void AddGoods(EGoodsType InDataId, int64 InAmount);
};
