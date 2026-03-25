// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Save/PlayerAccountData.h"
#include "Data/Save/ValkyrieSaveData.h"
#include "Data/Enum/DataEnums.h"
#include "SaveManager.generated.h"

class USaveGame;
class UValkyrieData;
class UItemData;
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
	FORCEINLINE class UAccountSaveGame* GetAccount() { return Account; }
	FORCEINLINE class UGachaSaveGame* GetGacha() { return Gacha; }
	FORCEINLINE class UGoodsSaveGame* GetGoods() { return Goods; }
	FORCEINLINE class UItemSaveGame* GetItem() { return Item; }
	FORCEINLINE class UStageSaveGame* GetStage() { return Stage; }
	FORCEINLINE class UUnitUpgradeSaveGame* GetUnitUpgrade() { return UnitUpgrade; }
	FORCEINLINE class UValkyrieSaveGame* GetValkyrie () { return Valkyrie; }

	FORCEINLINE bool IsNewAccount() { return bIsNewAccount; }
	
private:
	TMap <ESaveType, TFunction<void(USaveGame*)>> ActionSetData;
	TMap <ESaveType, TFunction<void(USaveGame*)>> ActionInitData;
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

	int32 LoadTask = 0;
	bool bIsNewAccount = false;
protected:
	void InitInitDataAction();
	void InitSetDataAction();
	void InitSaveDataAction();

	void InitDataInternal(ESaveType InSaveType, USaveGame* InLoadedData);
	void SetDataInternal(ESaveType InSaveType, USaveGame* InLoadedData);
	void SaveInternal(ESaveType InSaveType);

protected:
	void SetAccountData();
	void SetValkyrieData();
	void SetItemData();
	void SetGoodsData();

	//데이터 생성 후 저장용 Delgate 처리부분
	UFUNCTION()
	void OnDataLoaded(USaveGame* LoadedSaveGame, bool bIsSuccess, ESaveType InSaveType);

public:
	void InitAllData();
	int32 LoadAllData();

	bool IsAcountExist();

	void AddSaveItem(UItemData* InItem);
	void RemoveSaveItem(uint64 InUID);

	void CreateAccount(FString& InNickname);

	void LoadData(ESaveType InSaveType);
	void SaveData(ESaveType InSaveType);

	uint64 GetNextItemUID();
	uint64 GetNextValkyrieUID();

	void AddGoods(EGoodsType InGoodsType, int64 InAmount);

	void SaveValkyrie(UValkyrieData* InData);
};
