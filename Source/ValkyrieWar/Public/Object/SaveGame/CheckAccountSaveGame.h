// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CheckAccountSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCheckAccountSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	FORCEINLINE uint64 GetUserID() const { return UserId; }
	FORCEINLINE void SetUserID(uint64 InUserId) { UserId = InUserId; }

	FORCEINLINE uint64 GetItemUID()
	{
		ItemUID++;
		return ItemUID;
	}

	FORCEINLINE uint64 GetValkyrieUID()
	{
		ValkyrieUID++;
		return ValkyrieUID;
	}

protected:
	UPROPERTY()
	uint64 UserId = 0;

	UPROPERTY()
	uint64 ItemUID = 100000000;

	UPROPERTY()
	uint64 ValkyrieUID = 0;
};
