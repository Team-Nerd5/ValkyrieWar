// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/Data/GameDataFactory.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

UValkyrieData* UGameDataFactory::GenerateValkyrie(int32 InDataId, UGameInstance* InGameInstance)
{
	if (InGameInstance)
	{
		if (UDataManager* DataManager = InGameInstance->GetSubsystem<UDataManager>())
		{
			if (UValkyrieModule* Module = DataManager->GetValkyrieModule())
			{
				if (Module->HasValkyrie(InDataId))
				{
					//발키리 테이블 데이터에서 조각 데이터 아이디 호출
					//조각 아이템 생성
					// 
					//DataManager->GetItemModule()->AddItem(InDataId);
					//아이템 생성되었다고 이벤트..
				}
				else
				{
					int64 UID = Module->CreateValkyrie(InDataId);
					if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(InGameInstance))
					{
						EventSystem->Module.OnValkyrieGenerated.Broadcast(UID, Module->GetExistValkyrie(UID));
					}
				}

			}
		}
	}
	return nullptr;
}
