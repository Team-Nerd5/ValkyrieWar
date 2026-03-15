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
	CachedUnitData = UnitModule->GetUnitDataById(InUnitId);
	if (!CachedUnitData)
	{
		UE_LOG(LogTemp, Log, TEXT("유닛 데이터가 없습니다"));
		return;
	}
	int32 UnitLevel = CachedUnitData->GetLevel();
	if (UnitLevel < 1)
		return;

	int32 StatGroupId = 0;
	int32 Temp = CachedUnitData->GetLevel() % 3;
	switch (Temp)
	{
	case 1:
		StatGroupId = StatGroupId1;
		break;
	case 2:
		StatGroupId = StatGroupId2;
		break;
	case 0:
		StatGroupId = StatGroupId3;
		break;
	default:
		break;
	}

	if (UnitType)
	{
		UnitType->SetText(FText::FromString(TEXT("")));
	}
	if (CurrentLevel_Attack)
	{
		UE_LOG(LogTemp, Log, TEXT("Attack : %.1f"), CachedUnitData->GetStat(EStatusType::Attack));
		CurrentLevel_Attack->SetText(FText::AsNumber(CachedUnitData->GetStat(EStatusType::Attack)));
	}
	if (CurrentLevel_Health)
	{
		CurrentLevel_Health->SetText(FText::AsNumber(CachedUnitData->GetStat(EStatusType::Health)));
	}
	if (CurrentLevel_Defence)
	{
		CurrentLevel_Defence->SetText(FText::AsNumber(CachedUnitData->GetStat(EStatusType::Defence)));
	}

	auto NextLevelData = UnitUpgradeStatModule->GetStat(StatGroupId, UnitLevel + 1);
	if (NextLevel_Attack)
	{
		NextLevel_Attack->SetVisibility(NextLevelData.Attack > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Log, TEXT("Attack : %.1f"), NextLevelData.Attack);
		NextLevel_Attack->SetText(FText::AsNumber(NextLevelData.Attack));
	}
	if (NextLevel_Health)
	{
		NextLevel_Health->SetVisibility(NextLevelData.Health > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		NextLevel_Health->SetText(FText::AsNumber(NextLevelData.Health));
	}
	if (NextLevel_Defence)
	{
		NextLevel_Defence->SetVisibility(NextLevelData.Defence > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		NextLevel_Defence->SetText(FText::AsNumber(NextLevelData.Defence));
	}
}

void UUnitUpgradeBoxWidget::OnUpgradeUnit()
{
	if (!CachedUnitData)
		return;

	UnitModule->UnitLevelUpStat(UnitDataId);

	// 재화 소모 처리

	UpdateUpgradeInfo(UnitDataId);
}
