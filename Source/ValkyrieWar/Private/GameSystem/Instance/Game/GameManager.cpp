// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/GameManager.h"
#include "Data/Table/Widget/WidgetClassTableData.h"

TSubclassOf<UBaseWidget> UGameManager::GetUIClass(E_UITYPE InUIType)
{
    if (WidgetClassTable)
    {
        TArray<FWidgetClassTableData*> Rows;
        WidgetClassTable->GetAllRows(TEXT("SpawnInit"), Rows);

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
