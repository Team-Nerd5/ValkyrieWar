// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Engine/DataTable.h"
#include "Data/Enums.h"

#include "GameSystem/Base/BaseWidget.h"
#include "GameManager.generated.h"

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
		return CharacterUID++;
	}

	//저장데이터 로드되면 세팅용
	void UpdateCurrentUID(int64 InItemUID, int64 InCharacterUID);
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Data|Widget")
	TObjectPtr<UDataTable> WidgetClassTable = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Data|Map")
	TObjectPtr<UDataTable> MapDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Data|Table")
	TMap<ETableDataType, UDataTable*> GameDataTables;

private:
	uint64 ItemUID = 100000000;
	uint64 CharacterUID = 0;
};
