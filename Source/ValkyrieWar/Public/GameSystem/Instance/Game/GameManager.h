// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Engine/DataTable.h"
#include "Data/Enum/CommonEnums.h"
#include "Data/Enum/DataEnums.h"

#include "GameSystem/Base/BaseWidget.h"
#include "GameManager.generated.h"

class UValkyrieData;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGameManager : public UGameInstance
{
	GENERATED_BODY()

public:
	TSubclassOf<UBaseWidget> GetUIClass(EUIType InUIType);
	TSoftObjectPtr<UWorld> GetMapObject(EMapType InMapType);
	UDataTable* GetGameData(ETableDataType InType);

	FORCEINLINE int64 GetItemUID()
	{
		return ItemUID++;
	}
	FORCEINLINE int64 GetCharacterUID()
	{
		CharacterUID++;
		return CharacterUID;
	}
	UValkyrieData* const GetSelectedValkyrie();

	//저장데이터 로드되면 세팅용
	void UpdateCurrentUID(int64 InItemUID, int64 InCharacterUID);
	// 게임 인스턴트 초기화 함수
	virtual void Init() override;

	void SelectVakyrie(int64 InValkyrieUID);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Data|Widget")
	TObjectPtr<UDataTable> WidgetClassTable = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Data|Map")
	TObjectPtr<UDataTable> MapDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Data|Table")
	TMap<ETableDataType, TObjectPtr<UDataTable>> GameDataTables;

private:
	uint64 ItemUID = 100000000;
	uint64 CharacterUID = 0;

	uint64 SelectedValkyrieUID = 0;
};
