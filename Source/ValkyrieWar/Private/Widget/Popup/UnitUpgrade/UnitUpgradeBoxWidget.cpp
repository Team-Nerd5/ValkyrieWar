// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/UnitUpgrade/UnitUpgradeBoxWidget.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "Data/Game/UnitData.h"
#include "Data/Struct/StatValueData.h"

void UUnitUpgradeBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UnitUpgradeStatModule = DataManager->GetUnitUpgradeStatModule();
	UnitModule = DataManager->GetUnitModule();

	if (Btn_UpgradeUnit)
		Btn_UpgradeUnit->OnClicked.AddDynamic(this, &UUnitUpgradeBoxWidget::OnUpgradeUnit);
}

void UUnitUpgradeBoxWidget::Init(int32 InUnitId)
{
	UnitDataId = InUnitId;

	UpdateUpgradeInfo(UnitDataId);
}

void UUnitUpgradeBoxWidget::UpdateUpgradeInfo(int32 InUnitId)
{
	if (!UnitModule)
	{
		UE_LOG(LogTemp, Log, TEXT("유닛 모듈이 없습니다"));
		return;
	}
	CachedUnitData = UnitModule->GetUnitDataById(UnitDataId);
	if (!CachedUnitData.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("유닛 데이터가 없습니다"));
		return;
	}
	int32 UnitLevel = CachedUnitData->GetLevel();
	if (UnitLevel < 1)
		return;

	int32 StatId = CachedUnitData->GetTableData().StatId;

	FStatValueData CurrentLevelStatData = UnitUpgradeStatModule->GetTotalStat(StatId, UnitLevel);
	if (UnitType)
	{
		UnitType->SetText(FText::FromString(TEXT("")));
	}
	if (CurrentLevel_Attack)
	{
		UE_LOG(LogTemp, Log, TEXT("Attack : %.1f"), CurrentLevelStatData.Attack);
		CurrentLevel_Attack->SetText(FText::AsNumber(CurrentLevelStatData.Attack));
	}
	if (CurrentLevel_Health)
	{
		CurrentLevel_Health->SetText(FText::AsNumber(CurrentLevelStatData.Health));
	}
	if (CurrentLevel_Defence)
	{
		CurrentLevel_Defence->SetText(FText::AsNumber(CurrentLevelStatData.Defence));
	}

	auto NextLevelData = UnitUpgradeStatModule->GetStat(StatId, UnitLevel + 1);
	if (NextLevel_Attack)
	{
		if (NextLevelData.Attack > 0.0f)
			NextLevel_Attack->SetText(FText::AsNumber(CurrentLevelStatData.Attack + NextLevelData.Attack));
		else
			NextLevel_Attack->SetText(FText::AsNumber(CurrentLevelStatData.Attack));
	}
	if (NextLevel_Health)
	{
		if (NextLevelData.Health > 0.0f)
			NextLevel_Health->SetText(FText::AsNumber(CurrentLevelStatData.Health + NextLevelData.Health));
		else
			NextLevel_Health->SetText(FText::AsNumber(CurrentLevelStatData.Health));
	}
	if (NextLevel_Defence)
	{
		if (NextLevelData.Defence > 0.0f)
			NextLevel_Defence->SetText(FText::AsNumber(CurrentLevelStatData.Defence + NextLevelData.Defence));
		else
			NextLevel_Defence->SetText(FText::AsNumber(CurrentLevelStatData.Defence));
	}
}

void UUnitUpgradeBoxWidget::OnUpgradeUnit()
{
	if (!CachedUnitData.IsValid())
		return;

	CachedUnitData->LevelUp();
	int32 NewUnitLevel = CachedUnitData->GetLevel();

	// 업그레이드된 유닛 데이터 

	// 재화 소모 처리

	UpdateUpgradeInfo(UnitDataId);
}
