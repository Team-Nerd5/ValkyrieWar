// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"

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

UDataTable* UGameManager::GetGameData(ETableDataType InType)
{
    if (GameDataTables.Contains(InType))
    {
        return GameDataTables.FindChecked(InType);
    }

    return nullptr;
}

uint64 UGameManager::GetItemUID()
{
    if (USaveManager* Save = GetSubsystem<USaveManager>())
    {
        return Save->GetNextItemUID();
    }

    return 0;
}

uint64 UGameManager::GetValkyrieUID()
{
    if (USaveManager* Save = GetSubsystem<USaveManager>())
    {
        return Save->GetNextValkyrieUID();
    }

    return 0;
}

UValkyrieData* const UGameManager::GetSelectedValkyrie()
{
    if (UDataManager* DataManager = GetSubsystem<UDataManager>())
    {
        if (SelectedValkyrieUID > 0)
        {
            UValkyrieData* Data = DataManager->GetValkyrieModule()->GetExistValkyrie(SelectedValkyrieUID);
            if (Data)
            {
                return Data;
            }
        }

        //데이터가 없거나 UID가 0이면 문제가 있는거인데 0번 데이터를 반환
        return DataManager->GetValkyrieModule()->GetFirstValkyrie();
    }
    
    return nullptr;
}

UBlendSpace* UGameManager::GetValkyrieBlendSpace(EWeaponType InWeaponType)
{
    if (ValkyrieBlendSpace.Contains(InWeaponType))
    {
        return ValkyrieBlendSpace.FindRef(InWeaponType);
    }
    return nullptr;
}

void UGameManager::Init()
{
    Super::Init();

    //엑셀데이터 읽어오기
    if (UDataManager* DataManager = GetSubsystem<UDataManager>())
    {
        DataManager->CreateData();
    }
}

void UGameManager::SelectVakyrie(int64 InValkyrieUID)
{
    if (UDataManager* DataManager = GetSubsystem<UDataManager>())
    {
        SelectedValkyrieUID = InValkyrieUID;
        //SelectedValkyrie = DataManager->GetValkyrieModule()->GetExistValkyrie(InValkyrieUID);

        //만약에 존재하지 않으면
        //보유한것중에 앞에꺼 하나를 그냥 세팅해야할듯 하긴한데...
    }
}
