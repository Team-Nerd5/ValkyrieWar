// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Save/PlayerAccountData.h"
#include "Data/Enums.h"
#include "SaveManager.generated.h"

class USaveGame;
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
//	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
//	inline FPlayerAccountData GetPlayerAccountData() const { return CurrentPlayerAccountData; }
//
//	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
//	void SetPlayerAccountData(const FPlayerAccountData& InPlayerData);
//
//public: // 저장 / 로드
//	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
//	void SaveGame();
//
//	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
//	void LoadGame();
//
//protected:
//	UPROPERTY()
//	FString SaveSlotName = TEXT("PlayerSave01");
//
//	UPROPERTY()
//	int32 SaveIndex = 0;
//
//	UPROPERTY()
//	TObjectPtr<class UValkyrieWarSaveGame> CachedSaveGame = nullptr;
//
//protected: // 데이터
//	FPlayerAccountData CurrentPlayerAccountData;

public:
	//음..다 따로 들고있다면?
	TObjectPtr<class UCheckAccountSaveGame> CheckAccount = nullptr;
private:
	TMap <ESaveType, TFunction<void(USaveGame*)>> ActionSetData;


protected:

	void InitSetDataAction();

	void LoadDataInternal(ESaveType InSaveType, USaveGame* InLoadedData);

	UFUNCTION()
	void OnDataLoaded(USaveGame* LoadedSaveGame, bool bIsSuccess, ESaveType InSaveType);

public:
	void LoadAllData();

	void LoadCheckAccount();
};
