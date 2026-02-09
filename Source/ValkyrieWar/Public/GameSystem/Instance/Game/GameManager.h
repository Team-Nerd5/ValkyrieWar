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
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Data|Widget")
	TObjectPtr<UDataTable> WidgetClassTable = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Data|Map")
	TObjectPtr<UDataTable> MapDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Data|Table")
	TMap<ETableDataType, UDataTable*> GameDataTables;
};
