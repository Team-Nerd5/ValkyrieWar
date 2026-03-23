// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/Data/GameDataFactory.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

//만들기 전에 DataId 존재하는지 체크해서 있으면 아이템 생성해야 함...
UValkyrieData* UGameDataFactory::GenerateValkyrie(int32 InDataId, UGameInstance* InGameInstance)
{
	UValkyrieData* NewValkyrie = nullptr;
	if (InGameInstance)
	{
		if (UDataManager* DataManager = InGameInstance->GetSubsystem<UDataManager>())
		{
			if (UValkyrieModule* Module = DataManager->GetValkyrieModule())
			{
				NewValkyrie = Module->CreateValkyrie(InDataId);
				if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(InGameInstance))
				{
					EventSystem->Module.OnValkyrieGenerated.Broadcast(NewValkyrie->GetUID(), NewValkyrie);
				}
			}
		}
	}
	return NewValkyrie;
}
