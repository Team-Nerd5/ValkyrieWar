#include "Widget/Popup/Stage/StageDetailPopupWidget.h"
#include "Widget/Popup/Stage/EnemyInfoCardWidget.h"
#include "Widget/Popup/Stage/RewardInfoCardWidget.h"

#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"
#include "Components/WrapBoxSlot.h"

#include "GameSystem/Instance/Game/LevelManager.h"
#include "GameSystem/Instance/Game/UIManager.h"

void UStageDetailPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Close)
	{
		Button_Close->OnClicked.AddDynamic(this, &UStageDetailPopupWidget::HandleClickClose);
	}

	if (Button_Start)
	{
		Button_Start->OnClicked.AddDynamic(this, &UStageDetailPopupWidget::HandleClickStartStage);
	}
}

void UStageDetailPopupWidget::NativeDestruct()
{
	if (Button_Close)
	{
		Button_Close->OnClicked.RemoveDynamic(this, &UStageDetailPopupWidget::HandleClickClose);
	}

	if (Button_Start)
	{
		Button_Start->OnClicked.RemoveDynamic(this, &UStageDetailPopupWidget::HandleClickStartStage);
	}

	Super::NativeDestruct();
}

void UStageDetailPopupWidget::OpenUI()
{
	Super::OpenUI();
	RefreshUI();
}

void UStageDetailPopupWidget::CloseUI()
{
	Super::CloseUI();
}

void UStageDetailPopupWidget::SetStageDetail(const FStageDetailViewData& InDetailData)
{
	CachedDetailData = InDetailData;
	RefreshUI();
}

void UStageDetailPopupWidget::RefreshUI()
{
	if (Text_Title)
	{
		Text_Title->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("Chapter %d - Stage %d"),
					CachedDetailData.ChapterNum,
					CachedDetailData.StageNum
				)
			)
		);
	}

	if (WrapBox_EnemyList)
	{
		WrapBox_EnemyList->ClearChildren();

		if (EnemyInfoCardWidgetClass)
		{
			for (const FStageEnemyViewData& EnemyData : CachedDetailData.Enemies)
			{
				UEnemyInfoCardWidget* EnemyCard = CreateWidget<UEnemyInfoCardWidget>(this, EnemyInfoCardWidgetClass);
				if (!EnemyCard)
				{
					continue;
				}

				WrapBox_EnemyList->AddChildToWrapBox(EnemyCard);
				EnemyCard->SetEnemyData(EnemyData);
			}
		}
	}

	if (WrapBox_RewardList)
	{
		WrapBox_RewardList->ClearChildren();

		if (RewardInfoCardWidgetClass)
		{
			for (const FRewardViewData& RewardData : CachedDetailData.Rewards)
			{
				URewardInfoCardWidget* RewardCard = CreateWidget<URewardInfoCardWidget>(this, RewardInfoCardWidgetClass);
				if (!RewardCard)
				{
					continue;
				}

				RewardCard->SetRewardData(RewardData);

				if (UWrapBoxSlot* WrapSlot = WrapBox_RewardList->AddChildToWrapBox(RewardCard))
				{
					WrapSlot->SetPadding(FMargin(4.f));
					WrapSlot->SetFillEmptySpace(false);
				}
			}
		}
	}
}

void UStageDetailPopupWidget::HandleClickClose()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->CloseTopPopupUI();
	}
}

void UStageDetailPopupWidget::HandleClickStartStage()
{
	if (ULevelManager* LevelManager = GetGameInstance()->GetSubsystem<ULevelManager>())
	{
		LevelManager->LoadMap(EMapType::Battle, true);
	}
}
