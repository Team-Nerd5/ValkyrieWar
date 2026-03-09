// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Cheat/LobbyCheatManager.h"
#include "GameSystem/Instance/Game/InventorySystem.h"

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
