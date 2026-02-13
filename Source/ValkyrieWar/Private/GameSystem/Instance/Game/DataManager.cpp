// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/DataManager.h"

void UDataManager::InitTestItem(UGameManager* GameManager)
{
	if (!ItemModule)
		ItemModule = NewObject<UItemModule>(this);

	if(ItemModule && GameManager)
		ItemModule->Initialize(GameManager);
}
