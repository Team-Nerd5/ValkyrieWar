// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/UnitUpgrade/UnitUpgradeBoxWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "Data/Game/UnitData.h"
#include "Data/Module/GoodsModule.h"
#include "Data/Struct/StatValueData.h"

void UUnitUpgradeBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	EventSystem->Widget.OnGoodsUpdate.AddDynamic(this, &UUnitUpgradeBoxWidget::OnTestGoodsChangedAmount);

	if (Btn_UpgradeUnit)
		Btn_UpgradeUnit->OnClicked.AddDynamic(this, &UUnitUpgradeBoxWidget::OnUpgradeUnit);
}

void UUnitUpgradeBoxWidget::NativeDestruct()
{
	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	EventSystem->Widget.OnGoodsUpdate.RemoveDynamic(this, &UUnitUpgradeBoxWidget::OnTestGoodsChangedAmount);

	if (Btn_UpgradeUnit)
		Btn_UpgradeUnit->OnClicked.RemoveDynamic(this, &UUnitUpgradeBoxWidget::OnUpgradeUnit);

	Super::NativeDestruct();
}

void UUnitUpgradeBoxWidget::Init(TObjectPtr<UUnitData> InUnitData)
{
	if (!InUnitData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnitUpgradeBoxWidget] 유닛 데이터가 없습니다"));
		return;
	}
	CachedUnitData = InUnitData;
	CachedUnitDataId = InUnitData->GetDataId();

	// 위젯 표시 정보 초기화
	UpdateUpgradeInfo();

	// 업그레이드 버튼 비활성화 색상 설정
	FButtonStyle ButtonStyle = Btn_UpgradeUnit->GetStyle();
	ButtonStyle.Disabled.TintColor = FSlateColor(ButtonDisableColor);
	Btn_UpgradeUnit->SetStyle(ButtonStyle);
}

void UUnitUpgradeBoxWidget::UpdateUpgradeInfo()
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	if (!CachedUnitData)
		return;

	// 유닛 타입 표시
	if (UnitType)
	{
		FString EnumString = StaticEnum<EUnitCharacterType>()->GetNameStringByValue((int64)CachedUnitData->GetTableData().UnitType);
		UnitType->SetText(FText::FromString(EnumString));
	}
	// 유닛 아이콘 표시
	if (UnitIcon)
	{
		UTexture2D* Icon = CachedUnitData->GetIcon().LoadSynchronous();
		if (Icon)
		{
			UnitIcon->SetBrushFromTexture(Icon);
		}
	}
	// 현재 레벨
	if (UnitLevel)
	{
		UnitLevel->SetText(FText::AsNumber(CachedUnitData->GetLevel()));
	}

	// 현재 스텟
	if (CurrentLevel_Attack)
	{
		CurrentLevel_Attack->SetText(FText::AsNumber(
			CachedUnitData->GetStat(EStatusType::Attack) + DataManager->GetUnitModule()->GetUnitStat(CachedUnitData->GetDataId()).Attack));
	}
	if (CurrentLevel_Health)
	{
		CurrentLevel_Health->SetText(FText::AsNumber(
			CachedUnitData->GetStat(EStatusType::Health) + DataManager->GetUnitModule()->GetUnitStat(CachedUnitData->GetDataId()).Health));
	}
	if (CurrentLevel_Defence)
	{
		CurrentLevel_Defence->SetText(FText::AsNumber(
			CachedUnitData->GetStat(EStatusType::Defence) + DataManager->GetUnitModule()->GetUnitStat(CachedUnitData->GetDataId()).Defence));
	}

	// 레벨업 할 때 오를 스텟
	NextLevelData = DataManager->GetUnitUpgradeStatModule()->GetNextLevelData(CachedUnitData->GetLevelUpGroupId(), CachedUnitData->GetLevel() + 1);
	if (NextLevelData.IsValid())
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

	// 소모될 재화 아이콘
	if (NextLevel_CostTypeIcon)
	{
		if (NextLevelData.IsValid())
		{
			UTexture2D* CostTypeIcon = DataManager->GetGoodsModule()->GetTableData(NextLevelData->GetUpgradeCostType()).Icon.LoadSynchronous();
			NextLevel_CostTypeIcon->SetBrushFromTexture(CostTypeIcon);
		}
	}
	// 소모될 재화의 양
	if (NextLevel_Cost)
	{
		if (NextLevelData.IsValid())
			NextLevel_Cost->SetText(FText::AsNumber(NextLevelData->GetUpgradeCost()));
		else
		{
			NextLevel_Cost->SetVisibility(ESlateVisibility::Hidden);
			if (NextLevel_CostTypeIcon)
				NextLevel_CostTypeIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// 다음 레벨이 없다면 버튼 비활성화
	if (!NextLevelData.IsValid())
	{
		if (ButtonText)
		{
			ButtonText->SetText(FText::FromString(FString::Printf(TEXT("최대 레벨"))));
			SetEnableButton(false);
			return;
		}
	}

	CheckEnoughCost();
}

void UUnitUpgradeBoxWidget::OnTestGoodsChangedAmount(EGoodsType InGoodsType, uint64 InAmount)
{
	// 버튼 업데이트 테스트용 임시 바인딩(Goods위젯 업데이트 델리게이트 임시 사용)
	CheckEnoughCost();
}

void UUnitUpgradeBoxWidget::OnGoodsChangedAmount()
{
	// TODO: 재화의 변화가 있을 때 델리게이트를 통해 바인딩

	CheckEnoughCost();
}

void UUnitUpgradeBoxWidget::OnUpgradeUnit()
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	USaveManager* SaveManager = GetWorld()->GetGameInstance()->GetSubsystem<USaveManager>();
	if (!SaveManager)
		return;
	if (!NextLevelData.IsValid())
		return;

	// TODO: 재화 소모 추가 및 처리
	DataManager->GetGoodsModule()->Add(NextLevelData->GetUpgradeCostType(), -NextLevelData->GetUpgradeCost());

	DataManager->GetUnitModule()->UnitLevelUpStat(CachedUnitDataId);

	// 위젯 표시 정보 초기화
	UpdateUpgradeInfo();
}

void UUnitUpgradeBoxWidget::CheckEnoughCost()
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;

	if (NextLevelData.IsValid())
	{
		if (DataManager->GetGoodsModule()->IsEnough(NextLevelData->GetUpgradeCostType(), NextLevelData->GetUpgradeCost()))
		{
			SetEnableButton(true);
		}
		else
		{
			SetEnableButton(false);
		}
	}
}

void UUnitUpgradeBoxWidget::SetEnableButton(bool IsActive)
{
	if (Btn_UpgradeUnit)
	{
		if (IsActive)
		{
			Btn_UpgradeUnit->SetIsEnabled(true);
		}
		else
		{
			Btn_UpgradeUnit->SetIsEnabled(false);
		}
	}
}
