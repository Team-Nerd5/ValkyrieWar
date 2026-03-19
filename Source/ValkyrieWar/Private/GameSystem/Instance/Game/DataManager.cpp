// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/DataManager.h"

void UDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameManager = Cast<UGameManager>(GetGameInstance());

}

int32 UDataManager::CreateData()
{
	UE_LOG(LogTemp, Log, TEXT("TableData Start Create"));
	TaskCount = 0;

	InitializeData(GoodsModule);
	InitializeData(ContentsModule);
	InitializeData(StatGroupModule);
	InitializeData(ProjectileModule);

	//Skill, Attack 에서 사용중이라 먼저 해야함.
	InitializeData(TargetingModule);
	InitializeData(SkillEffectModule);

	InitializeData(AttackModule);
	InitializeData(SkillModule);

	InitializeData(ItemModule);
	InitializeData(ValkyrieModule);
	InitializeData(UnitModule);
	InitializeData(TopMenuModule);
	InitializeData(UnitUpgradeStatModule);

	InitializeData(StageInfoModule);
	InitializeData(StageModule);

	InitializeData(GachaRandomModule);

	return TaskCount;
}
