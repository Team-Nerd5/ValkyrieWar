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

void ULobbyCheatManager::AddAll()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UDataManager* DataManager = World->GetGameInstance()->GetSubsystem<UDataManager>())
		{
			DataManager->GetGoodsModule()->Add(EGoodsType::Gold, 10000000);
			DataManager->GetGoodsModule()->Add(EGoodsType::Gem, 10000000);
		}

		if (UInventorySystem* InventorySystem = World->GetGameInstance()->GetSubsystem<UInventorySystem>())
		{
			InventorySystem->AddItem(310001, 100);
			InventorySystem->AddItem(310002, 100);
			InventorySystem->AddItem(310003, 100);
			InventorySystem->AddItem(310004, 100);
			InventorySystem->AddItem(310005, 100);
			InventorySystem->AddItem(310006, 100);

			InventorySystem->AddItem(411021, 1);
			InventorySystem->AddItem(412021, 1);
			InventorySystem->AddItem(413021, 1);
			InventorySystem->AddItem(414021, 1);
			InventorySystem->AddItem(415021, 1);
			InventorySystem->AddItem(416021, 1);
			InventorySystem->AddItem(417021, 1);
			InventorySystem->AddItem(418021, 1);

			InventorySystem->AddItem(440011, 1);
			InventorySystem->AddItem(440021, 1);
			InventorySystem->AddItem(440031, 1);
			InventorySystem->AddItem(450011, 1);
			InventorySystem->AddItem(450021, 1);
			InventorySystem->AddItem(450031, 1);
			InventorySystem->AddItem(460011, 1);
			InventorySystem->AddItem(460021, 1);
			InventorySystem->AddItem(460031, 1);
			InventorySystem->AddItem(470011, 1);
			InventorySystem->AddItem(470021, 1);
			InventorySystem->AddItem(470031, 1);
			InventorySystem->AddItem(470041, 1);
			InventorySystem->AddItem(480011, 1);
			InventorySystem->AddItem(480021, 1);
			InventorySystem->AddItem(480031, 1);
			InventorySystem->AddItem(480041, 1);
		}
	}
}
