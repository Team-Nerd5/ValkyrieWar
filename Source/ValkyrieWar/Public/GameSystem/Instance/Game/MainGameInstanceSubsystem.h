// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
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

protected:
	UPROPERTY()
	int32 Gold = 0;

	UPROPERTY()
	int32 Ticket = 0;

	UPROPERTY()
	FString SaveSlotName = TEXT("PlayerSave01");

	UPROPERTY()
	int32 SaveIndex = 0;

protected:
	UPROPERTY()
	TObjectPtr<class UMainSaveGame> CachedSaveGame = nullptr;

};
