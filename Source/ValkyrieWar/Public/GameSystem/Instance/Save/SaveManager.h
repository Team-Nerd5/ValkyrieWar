// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Save/PlayerAccountData.h"
#include "Data/Enum/DataEnums.h"
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

public:
	FORCEINLINE class UCheckAccountSaveGame* GetCheckAccount() { return CheckAccount; }
	FORCEINLINE class UAccountSaveGame* GeAccount() { return Account; }
	FORCEINLINE class UGachaSaveGame* GetGacha() { return Gacha; }
	FORCEINLINE class UGoodsSaveGame* GetGoods() { return Goods; }
	FORCEINLINE class UItemSaveGame* GetItem() { return Item; }
	FORCEINLINE class UStageSaveGame* GetStage() { return Stage; }
	FORCEINLINE class UUnitUpgradeSaveGame* GetUnitUpgrade() { return UnitUpgrade; }
	FORCEINLINE class UValkyrieSaveGame* GetValkyrie () { return Valkyrie; }

private:
	TMap <ESaveType, TFunction<void(USaveGame*)>> ActionSetData;
	TMap <ESaveType, TFunction<void()>> ActionSaveData;

	UPROPERTY()
	TObjectPtr<class UCheckAccountSaveGame> CheckAccount = nullptr;
	UPROPERTY()
	TObjectPtr<class UAccountSaveGame> Account = nullptr;
	UPROPERTY()
	TObjectPtr<class UGachaSaveGame> Gacha = nullptr;
	UPROPERTY()
	TObjectPtr<class UGoodsSaveGame> Goods = nullptr;
	UPROPERTY()
	TObjectPtr<class UItemSaveGame> Item = nullptr;
	UPROPERTY()
	TObjectPtr<class UStageSaveGame> Stage = nullptr;
	UPROPERTY()
	TObjectPtr<class UUnitUpgradeSaveGame> UnitUpgrade = nullptr;
	UPROPERTY()
	TObjectPtr<class UValkyrieSaveGame> Valkyrie = nullptr;


protected:

	void InitSetDataAction();
	void InitSaveDataAction();

	void LoadDataInternal(ESaveType InSaveType, USaveGame* InLoadedData);
	/// <summary>
	/// 실제 Save파일로 암호화 저장
	/// </summary>
	/// <param name="InSaveType"></param>
	void SaveInternal(ESaveType InSaveType);

	UFUNCTION()
	void OnDataLoaded(USaveGame* LoadedSaveGame, bool bIsSuccess, ESaveType InSaveType);

public:
	void LoadAllData();

	//게임 로드 1순위 계정 체크
	bool IsAcountExist();

	//게임로드 2순위 id 있으면 가져오고, 음..
	uint64 GetUserId();

	void SaveData(ESaveType InSaveType);
};
