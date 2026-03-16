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
		return;
	if (!UnitUpgradeStatModule)
		return;
	CachedUnitData = UnitModule->GetUnitDataById(InUnitId);
	if (!CachedUnitData)
		return;
	int32 Level = CachedUnitData->GetLevel();
	if (Level < 1)
		return;


	if (UnitType)
	{
		FString EnumString = StaticEnum<EUnitCharacterType>()->GetNameStringByValue((int64)CachedUnitData->GetTableData().UnitType);
		UnitType->SetText(FText::FromString(EnumString));
	}
	if (UnitLevel)
	{
		UnitLevel->SetText(FText::AsNumber(Level));
	}

	if (CurrentLevel_Attack)
	{
		CurrentLevel_Attack->SetText(FText::AsNumber(CachedUnitData->GetStat(EStatusType::Attack) + UnitModule->GetUnitStat(UnitDataId).Attack));
	}
	if (CurrentLevel_Health)
	{
		CurrentLevel_Health->SetText(FText::AsNumber(CachedUnitData->GetStat(EStatusType::Health) + UnitModule->GetUnitStat(UnitDataId).Health));
	}
	if (CurrentLevel_Defence)
	{
		CurrentLevel_Defence->SetText(FText::AsNumber(CachedUnitData->GetStat(EStatusType::Defence) + UnitModule->GetUnitStat(UnitDataId).Defence));
	}

	auto NextLevelData = UnitUpgradeStatModule->GetStat(UnitUpgradeStatModule->GetStatGroupId(Level + 1), Level + 1);
	if (NextLevel_Attack)
	{
		NextLevel_Attack->SetVisibility(NextLevelData.Attack > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		NextLevel_Attack->SetText(FText::FromString(FString::Printf(TEXT(" + %.0f"), NextLevelData.Attack)));
	}
	if (NextLevel_Health)
	{
		NextLevel_Health->SetVisibility(NextLevelData.Health > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		NextLevel_Health->SetText(FText::FromString(FString::Printf(TEXT(" + %.0f"), NextLevelData.Health)));
	}
	if (NextLevel_Defence)
	{
		NextLevel_Defence->SetVisibility(NextLevelData.Defence > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		NextLevel_Defence->SetText(FText::FromString(FString::Printf(TEXT(" + %.0f"), NextLevelData.Defence)));
	}

	if (NextLevel_Cost)
	{
		// TODO: 업그레이드 코스트 UI업데이트

		NextLevel_Cost->SetText(FText::FromString(FString::Printf(TEXT("0"))));
	}

	if (Level >= 10)
	{
		if (Btn_UpgradeUnit)
		{
			Btn_UpgradeUnit->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UUnitUpgradeBoxWidget::OnUpgradeUnit()
{
	// TODO: 재화 소모 조건 추가 및 처리

	UnitModule->UnitLevelUpStat(UnitDataId);

	UpdateUpgradeInfo(UnitDataId);
}
