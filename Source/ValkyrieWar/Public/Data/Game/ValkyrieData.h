// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Data/Enum/CharacterEnums.h"

#include "Data/Table/GameData/ValkyrieDataRow.h"

#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
#include "Data/Game/ItemData.h"

#include "GameSystem/Instance/Game/GameManager.h"
#include "ValkyrieData.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieData : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	uint64 UID = 0;

	UPROPERTY()
	FValkyrieDataRow TableData;

	UPROPERTY()
	TObjectPtr<UAttackData> AttackData = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<USkillData>> SkillData;

	//기본 데이터 세팅
	void MakeData(const FValkyrieDataRow InTableData, UGameManager* InGameManager);

	UPROPERTY()
	TMap<EStatusType, float> Stat;

public:
	//장착된 무기 세팅
	void UpdateWeapon(UItemData* InNewWeapon, UGameManager* InGameManager);

	//새로 캐릭터를 획득
	void Initialize(const FValkyrieDataRow InTableData, UGameManager* InGameManager);

	//저장된 데이터 로드 시(모듈에서 호출되는걸로 하자...)
	void LoadData(uint64 InUID, const FValkyrieDataRow InTableData, UGameManager* InGameManager);

	FORCEINLINE const uint64 GetUID() { return UID; }
	FORCEINLINE const int32 GetDataID() { return TableData.DataId; }
	FORCEINLINE TArray<USkillData*> GetSkillData() { return SkillData;}
	FORCEINLINE UAttackData* GetAttackData() { return AttackData; }
	FORCEINLINE TSubclassOf<class AValkyrieCharacter> GetSpawnClass() { return TableData.SpawnClass; }
	FORCEINLINE float GetStat(EStatusType InType) { return *Stat.Find(InType); }
};
