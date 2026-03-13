// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "BattleCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBattleCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(exec)
	void EquipItem(int32 InDataId);
};
