// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/PlayerSaveData.h"
#include "Data/Enums.h"
#include "MainGameInstanceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelTransitStarted, EMapType, TargetMapType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelTransitCompleted, EMapType, LoadedMapType);

// 세이브 파일 저장경로 : ValkyrieWar/Saved/SaveGames 안에 있는 .sav 파일

UCLASS()
class VALKYRIEWAR_API UMainGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public: // 게터 / 세터
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	inline FPlayerSaveData GetPlayerData() const { return CurrentPlayerData; }

	UFUNCTION(BlueprintCallable, Category = "LevelSystem")
	inline EMapType GetCurrentMapType() const { return CurrentMapType; }

	UFUNCTION(BlueprintCallable, Category = "LoginSystem")
	bool GetIsLoggedIn() const { return bIsLoggedIn; }

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SetPlayerData(const FPlayerSaveData& InPlayerData);

public: // 저장 / 로드
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void AddGold(int32 InGold);

	UFUNCTION(BlueprintCallable)
	void AddTicket(int32 InTicket);

public: // 레벨 이동
	UFUNCTION(BlueprintCallable, Category = "LevelSystem")
	void TransitLevel(EMapType MapType);

	UFUNCTION()
	void LevelTransitComplete();

public: // 로그인 관리
	UFUNCTION(BlueprintCallable, Category = "LoginSystem")
	void LoginPlayer(const FString& InPlayerName);

public: // 델리게이트
	UPROPERTY()
	FOnLevelTransitStarted OnLevelTransitStarted;

	UPROPERTY()
	FOnLevelTransitCompleted OnLevelTransitCompleted;

protected: // 변수
	UPROPERTY()
	FString SaveSlotName = TEXT("PlayerSave01");

	UPROPERTY()
	int32 SaveIndex = 0;

	UPROPERTY()
	TObjectPtr<class UMainSaveGame> CachedSaveGame = nullptr;

	UPROPERTY()
	FPlayerSaveData CurrentPlayerData;

	UPROPERTY()
	EMapType CurrentMapType = EMapType::Login;

	UPROPERTY()
	bool bIsLoggedIn = false;

};
