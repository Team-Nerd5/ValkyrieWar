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
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;

	if (!CachedUnitData || UnitDataId != InUnitId)
	{
		CachedUnitData = DataManager->GetUnitModule()->GetUnitDataById(InUnitId);
	}
	if (!CachedUnitData)
		return;

	if (UnitType)
	{
		FString EnumString = StaticEnum<EUnitCharacterType>()->GetNameStringByValue((int64)CachedUnitData->GetTableData().UnitType);
		UnitType->SetText(FText::FromString(EnumString));
	}
	if (UnitIcon)
	{
		UTexture2D* Icon = CachedUnitData->GetIcon().LoadSynchronous();
		if (Icon)
		{
			UnitIcon->SetBrushFromTexture(Icon);
		}
	}
	if (UnitLevel)
	{
		UnitLevel->SetText(FText::AsNumber(CachedUnitData->GetLevel()));
	}

	if (CurrentLevel_Attack)
	{
		CurrentLevel_Attack->SetText(FText::AsNumber(
			CachedUnitData->GetStat(EStatusType::Attack) + DataManager->GetUnitModule()->GetUnitStat(UnitDataId).Attack));
	}
	if (CurrentLevel_Health)
	{
		CurrentLevel_Health->SetText(FText::AsNumber(
			CachedUnitData->GetStat(EStatusType::Health) + DataManager->GetUnitModule()->GetUnitStat(UnitDataId).Health));
	}
	if (CurrentLevel_Defence)
	{
		CurrentLevel_Defence->SetText(FText::AsNumber(
			CachedUnitData->GetStat(EStatusType::Defence) + DataManager->GetUnitModule()->GetUnitStat(UnitDataId).Defence));
	}

	UUnitUpgradeData* NextLevelData = DataManager->GetUnitUpgradeStatModule()->GetNextLevelData(CachedUnitData->GetLevelUpGroupId(), CachedUnitData->GetLevel() + 1);
	if (NextLevelData)
	{
		if (NextLevel_Attack)
		{
			NextLevel_Attack->SetVisibility(NextLevelData->GetAttack() > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			NextLevel_Attack->SetText(FText::FromString(FString::Printf(TEXT(" + %.d"), NextLevelData->GetAttack())));
		}
		if (NextLevel_Health)
		{
			NextLevel_Health->SetVisibility(NextLevelData->GetHealth() > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			NextLevel_Health->SetText(FText::FromString(FString::Printf(TEXT(" + %.d"), NextLevelData->GetHealth())));
		}
		if (NextLevel_Defence)
		{
			NextLevel_Defence->SetVisibility(NextLevelData->GetDefence() > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			NextLevel_Defence->SetText(FText::FromString(FString::Printf(TEXT(" + %.d"), NextLevelData->GetDefence())));
		}
	}
	else
	{
		if (NextLevel_Attack)
			NextLevel_Attack->SetVisibility(ESlateVisibility::Hidden);
		if (NextLevel_Health)
			NextLevel_Health->SetVisibility(ESlateVisibility::Hidden);
		if (NextLevel_Defence)
			NextLevel_Defence->SetVisibility(ESlateVisibility::Hidden);
	}
	

	if (NextLevel_Cost)
	{
		// TODO: 업그레이드 코스트 UI업데이트

		NextLevel_Cost->SetText(FText::FromString(FString::Printf(TEXT("0"))));
	}

	if (!NextLevelData)
	{
		if (Btn_UpgradeUnit)
		{
			// 비활성화 색상 설정
			FButtonStyle ButtonStyle = Btn_UpgradeUnit->GetStyle();
			ButtonStyle.Disabled.TintColor = FSlateColor(ButtonBackGroundColor);
			Btn_UpgradeUnit->SetStyle(ButtonStyle);

			Btn_UpgradeUnit->SetIsEnabled(false);
		}
		if (ButtonText)
		{
			ButtonText->SetText(FText::FromString(FString::Printf(TEXT("최대 레벨"))));
		}
	}
	
}

void UUnitUpgradeBoxWidget::OnUpgradeUnit()
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;

	// TODO: 재화 소모 조건 추가 및 처리

	DataManager->GetUnitModule()->UnitLevelUpStat(UnitDataId);

	UpdateUpgradeInfo(UnitDataId);
}
