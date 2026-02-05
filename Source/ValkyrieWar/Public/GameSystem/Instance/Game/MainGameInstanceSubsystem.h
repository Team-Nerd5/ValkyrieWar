// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/PlayerSaveData.h"
#include "MainGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UMainGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	UFUNCTION()
	void SaveGame();

	UFUNCTION()
	void LoadGame();

	UFUNCTION()
	void AddGold(int32 InGold);

	UFUNCTION()
	void AddTicket(int32 InTicket);

	UFUNCTION()
	inline FPlayerSaveData GetPlayerData() const { return CurrentPlayerData; }

	UFUNCTION()
	void SetPlayerData(const FPlayerSaveData& InPlayerData);
	 
protected:
	UPROPERTY()
	FString SaveSlotName = TEXT("PlayerSave01");

	UPROPERTY()
	int32 SaveIndex = 0;

	UPROPERTY()
	TObjectPtr<class UMainSaveGame> CachedSaveGame = nullptr;

	UPROPERTY()
	FPlayerSaveData CurrentPlayerData;

};
