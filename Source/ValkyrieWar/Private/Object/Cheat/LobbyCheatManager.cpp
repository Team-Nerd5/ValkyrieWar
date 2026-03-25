// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Cheat/LobbyCheatManager.h"

#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/Game/DataManager.h"

void ULobbyCheatManager::AddItem(int32 InItemID, int32 InAmount)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UInventorySystem* InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>())
		{
			InventorySystem->AddItem(InItemID, InAmount);
		}
	}	
}

void ULobbyCheatManager::AddGoods(EGoodsType InDataType, int64 InAmount)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UDataManager* DataManager = World->GetGameInstance()->GetSubsystem<UDataManager>())
		{
			DataManager->GetGoodsModule()->Add(InDataType, InAmount);
		}
	}
}
