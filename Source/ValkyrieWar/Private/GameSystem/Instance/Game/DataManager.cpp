// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/DataManager.h"

void UDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameManager = Cast<UGameManager>(GetGameInstance());

	CreateData();
}

void UDataManager::CreateData()
{
	UE_LOG(LogTemp, Log, TEXT("TableData Start Create"));
	//음.. 그냥 순서대로 해야하나..?
	InitializeData(ItemModule);
	InitializeData(ValkyrieModule);

	//Skill, Attack 에서 사용중이라 먼저 해야함.
	InitializeData(SkillEffectModule);

	InitializeData(AttackModule);
	InitializeData(SkillModule);

}
