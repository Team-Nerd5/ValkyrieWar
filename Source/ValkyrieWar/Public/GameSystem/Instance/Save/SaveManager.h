// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Save/PlayerAccountData.h"
#include "SaveManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API USaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public: // 게터 / 세터
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	inline FPlayerAccountData GetPlayerAccountData() const { return CurrentPlayerAccountData; }

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SetPlayerAccountData(const FPlayerAccountData& InPlayerData);

public: // 저장 / 로드
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame();

protected:
	UPROPERTY()
	FString SaveSlotName = TEXT("PlayerSave01");

	UPROPERTY()
	int32 SaveIndex = 0;

	UPROPERTY()
	TObjectPtr<class UValkyrieWarSaveGame> CachedSaveGame = nullptr;

	FPlayerAccountData CurrentPlayerAccountData;
};
