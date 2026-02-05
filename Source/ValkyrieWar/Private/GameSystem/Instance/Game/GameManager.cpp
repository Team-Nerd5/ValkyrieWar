// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/GameManager.h"
#include "Data/Table/Widget/WidgetClassTableData.h"
#include "Data/Table/Map/MapLinkTableData.h"
#include <Kismet/GameplayStatics.h>

TSubclassOf<UBaseWidget> UGameManager::GetUIClass(EUIType InUIType)
{
    if (WidgetClassTable)
    {
        TArray<FWidgetClassTableData*> Rows;
        WidgetClassTable->GetAllRows(TEXT("WidgeInit"), Rows);

        for (FWidgetClassTableData* row : Rows)
        {
            if (row->UIType == InUIType)
            {
                return row->WidgetClass.LoadSynchronous();
            }
        }
    }

    return nullptr;
}

TSoftObjectPtr<UWorld> UGameManager::GetMapObject(EMapType InMapType)
{
    if (MapDataTable)
    {
        TArray<FMapLinkTableData*> Rows;
        MapDataTable->GetAllRows(TEXT("MapInit"), Rows);

        for (FMapLinkTableData* row : Rows)
        {
            if (row->UIType == InMapType)
            {
                return row->Map.LoadSynchronous();
            }
        }
    }

    return nullptr;
}
