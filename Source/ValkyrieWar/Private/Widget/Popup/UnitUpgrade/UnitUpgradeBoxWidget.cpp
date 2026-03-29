// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/UnitUpgrade/UnitUpgradeBoxWidget.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"

#include "Data/Game/UnitData.h"
#include "Data/Module/GoodsModule.h"
#include "Data/Struct/StatValueData.h"

void UUnitUpgradeBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 업데이트 테스트용 임시 바인딩(Goods위젯 업데이트 델리게이트 임시 사용)
	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	EventSystem->Widget.OnGoodsUpdate.AddDynamic(this, &UUnitUpgradeBoxWidget::OnGoodsChangedAmount);

	if (Btn_UpgradeUnit)
		Btn_UpgradeUnit->OnClicked.AddDynamic(this, &UUnitUpgradeBoxWidget::OnUpgradeUnit);
}

void UUnitUpgradeBoxWidget::NativeDestruct()
{
	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	EventSystem->Widget.OnGoodsUpdate.RemoveDynamic(this, &UUnitUpgradeBoxWidget::OnGoodsChangedAmount);

	if (Btn_UpgradeUnit)
		Btn_UpgradeUnit->OnClicked.RemoveDynamic(this, &UUnitUpgradeBoxWidget::OnUpgradeUnit);

	Super::NativeDestruct();
}

void UUnitUpgradeBoxWidget::Init(int32 InUnitDataId)
{
	CachedUnitDataId = InUnitDataId;

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
	UUnitModule* UnitModule = DataManager->GetUnitModule();
	if (!UnitModule)
		return;
	UUnitData* UnitData = UnitModule->GetUnitDataById(CachedUnitDataId);
	if (!UnitData)
		return;
	UUnitUpgradeStatModule* UnitUpgradeStatMoudle = DataManager->GetUnitUpgradeStatModule();
	if (!UnitUpgradeStatMoudle)
		return;
	UGoodsModule* GoodsModule = DataManager->GetGoodsModule();
	if (!GoodsModule)
		return;

	// 유닛 타입 표시
	if (UnitType)
	{
		FString EnumString = StaticEnum<EUnitCharacterType>()->GetNameStringByValue((int64)UnitData->GetTableData().UnitType);
		UnitType->SetText(FText::FromString(EnumString));
		UnitType->SetColorAndOpacity(SetTextByGrade(UnitData->GetCurrentGrade()));
	}
	// 유닛 아이콘 표시
	if (UnitIcon)
	{
		UTexture2D* Icon = UnitData->GetIcon().LoadSynchronous();
		if (Icon)
		{
			UnitIcon->SetBrushFromTexture(Icon);
		}
	}
	// 현재 레벨
	if (UnitLevel)
	{
		UnitLevel->SetText(FText::AsNumber(UnitData->GetLevel()));
	}

	// 현재 스텟
	if (CurrentLevel_Attack)
	{
		CurrentLevel_Attack->SetText(FText::AsNumber(UnitData->GetTotalStat(EStatusType::Attack)));
	}
	if (CurrentLevel_Health)
	{
		CurrentLevel_Health->SetText(FText::AsNumber(UnitData->GetTotalStat(EStatusType::Health)));
	}
	if (CurrentLevel_Defence)
	{
		CurrentLevel_Defence->SetText(FText::AsNumber(UnitData->GetTotalStat(EStatusType::Defence)));
	}

	if (UUnitUpgradeData* UpgradeData = UnitUpgradeStatMoudle->GetNextLevelData(UnitData->GetLevelUpGroupId(), UnitData->GetLevel() + 1))
	{
		if (UpgradeData && UpgradeData->GetUpgradeCostType() != EGoodsType::None)
		{
			UseGoodsType = UpgradeData->GetUpgradeCostType();
			UseGoodsCost = UpgradeData->GetUpgradeCost();

			// 레벨업 할 때 오를 스텟
			if (NextLevel_Attack)
			{
				NextLevel_Attack->SetVisibility(UpgradeData->GetAttack() > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
				NextLevel_Attack->SetText(FText::FromString(FString::Printf(TEXT(" + %.d"), UpgradeData->GetAttack())));
			}
			if (NextLevel_Health)
			{
				NextLevel_Health->SetVisibility(UpgradeData->GetHealth() > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
				NextLevel_Health->SetText(FText::FromString(FString::Printf(TEXT(" + %.d"), UpgradeData->GetHealth())));
			}
			if (NextLevel_Defence)
			{
				NextLevel_Defence->SetVisibility(UpgradeData->GetDefence() > 0.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
				NextLevel_Defence->SetText(FText::FromString(FString::Printf(TEXT(" + %.d"), UpgradeData->GetDefence())));
			}

			// 소모될 재화 아이콘
			if (NextLevel_CostTypeIcon)
			{
				UTexture2D* CostTypeIcon = GoodsModule->GetTableData(UpgradeData->GetUpgradeCostType()).Icon.LoadSynchronous();
				NextLevel_CostTypeIcon->SetBrushFromTexture(CostTypeIcon);
			}

			// 소모될 재화의 양
			if (NextLevel_Cost)
			{
				if (UpgradeData)
					NextLevel_Cost->SetText(FText::AsNumber(UpgradeData->GetUpgradeCost()));
				else
				{
					NextLevel_Cost->SetVisibility(ESlateVisibility::Hidden);
					if (NextLevel_CostTypeIcon)
						NextLevel_CostTypeIcon->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
		IsMaxUpgrade = false;
	}
	else
	{
		// 오를 스텟 텍스트 숨김
		if (NextLevel_Attack)
			NextLevel_Attack->SetVisibility(ESlateVisibility::Hidden);
		if (NextLevel_Health)
			NextLevel_Health->SetVisibility(ESlateVisibility::Hidden);
		if (NextLevel_Defence)
			NextLevel_Defence->SetVisibility(ESlateVisibility::Hidden);

		// 사용될 Cost 와 CostIcon 숨김
		if (NextLevel_CostTypeIcon)
			NextLevel_CostTypeIcon->SetVisibility(ESlateVisibility::Hidden);
		if (NextLevel_Cost)
			NextLevel_Cost->SetVisibility(ESlateVisibility::Hidden);


		// 다음 레벨이 없다면 버튼 비활성화
		if (ButtonText)
		{
			ButtonText->SetText(FText::FromString(FString::Printf(TEXT("최대 레벨"))));
		}
		if (Btn_UpgradeUnit)
		{
			Btn_UpgradeUnit->SetIsEnabled(false);
		}
		IsMaxUpgrade = true;
	}
	CheckEnoughCost();
}

void UUnitUpgradeBoxWidget::OnGoodsChangedAmount(EGoodsType InGoodsType, uint64 InAmount)
{
	CheckEnoughCost();
}

void UUnitUpgradeBoxWidget::OnUpgradeUnit()
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UGoodsModule* GoodsModule = DataManager->GetGoodsModule();
	if (!GoodsModule)
		return;
	UUnitModule* UnitModule = DataManager->GetUnitModule();
	if (!UnitModule)
		return;

	// 재화 사용 처리
	GoodsModule->Add(UseGoodsType, -UseGoodsCost);

	// 유닛 스텟 업그레이드
	UnitModule->LevelUpUnit(CachedUnitDataId);

	UpdateUpgradeInfo();
}

void UUnitUpgradeBoxWidget::CheckEnoughCost()
{
	UDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManager>();
	if (!DataManager)
		return;
	UGoodsModule* GoodsModule = DataManager->GetGoodsModule();
	if (!GoodsModule)
		return;

	if (Btn_UpgradeUnit)
	{
		if (!IsMaxUpgrade)
		{
			if (GoodsModule->IsEnough(UseGoodsType, UseGoodsCost))
			{
				Btn_UpgradeUnit->SetIsEnabled(true);
			}
			else
			{
				Btn_UpgradeUnit->SetIsEnabled(false);
			}
		}
	}
}

FSlateColor UUnitUpgradeBoxWidget::SetTextByGrade(EGradeType InUnitGradeType)
{
	FSlateColor TextColor;
	switch (InUnitGradeType)
	{
	case EGradeType::Common:
		TextColor = FSlateColor(FLinearColor::White);
		break;
	case EGradeType::Uncommon:
		TextColor = FSlateColor(FLinearColor::Green);
		break;
	case EGradeType::Rare:
		TextColor = FSlateColor(FLinearColor::Blue);
		break;
	case EGradeType::Unique:
		TextColor = FSlateColor(FLinearColor(1.0, 0.5, 0.0));	// Orenge
		break;
	case EGradeType::Legend:
		TextColor = FSlateColor(FLinearColor::Red);
		break;
	default:
		break;
	}
	return TextColor;
}
