#include "Widget/Popup/Stage/StageListPanelWidget.h"
#include "Widget/Popup/Stage/StageItemWidget.h"
#include "Widget/Popup/Stage/StageDetailPopupWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/LevelManager.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "Data/Module/StageModule.h"
#include "Data/Module/StageInfoModule.h"
#include "Data/Struct/StageEntryData.h"
#include "Data/Struct/StageDetailViewData.h"

void UStageListPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_PrevChapter)
	{
		Button_PrevChapter->OnClicked.AddDynamic(this, &UStageListPanelWidget::HandlePrevChapter);
	}

	if (Button_NextChapter)
	{
		Button_NextChapter->OnClicked.AddDynamic(this, &UStageListPanelWidget::HandleNextChapter);
	}
}

void UStageListPanelWidget::NativeDestruct()
{
	if (Button_PrevChapter)
	{
		Button_PrevChapter->OnClicked.RemoveDynamic(this, &UStageListPanelWidget::HandlePrevChapter);
	}

	if (Button_NextChapter)
	{
		Button_NextChapter->OnClicked.RemoveDynamic(this, &UStageListPanelWidget::HandleNextChapter);
	}

	SpawnedStageItems.Empty();

	Super::NativeDestruct();
}

void UStageListPanelWidget::OpenUI()
{
	UIType = EUIType::PopupStageList;
	UILayer = EUILayer::POPUP;

	Super::OpenUI();

	if (UStageModule* StageModule = GetStageModule())
	{
		if (CurrentChapter <= 0 || !StageModule->HasChapter(CurrentChapter))
		{
			CurrentChapter = StageModule->GetMinChapter();
		}
	}

	RefreshUI();
}

void UStageListPanelWidget::CloseUI()
{
	ClearStageItems();
	Super::CloseUI();
}

void UStageListPanelWidget::RefreshUI()
{
	RefreshChapterHeader();
	RefreshChapterButtons();
	RebuildStageItems();
}

UStageModule* UStageListPanelWidget::GetStageModule() const
{
	if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
	{
		if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
		{
			return DataManager->GetStageModule();
		}
	}

	return nullptr;
}

UStageInfoModule* UStageListPanelWidget::GetStageInfoModule() const
{
	if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
	{
		if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
		{
			return DataManager->GetStageInfoModule();
		}
	}

	return nullptr;
}

UStageRewardModule* UStageListPanelWidget::GetStageRewardModule() const
{
	if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
	{
		if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
		{
			return DataManager->GetStageRewardModule();
		}
	}

	return nullptr;
}

UUnitModule* UStageListPanelWidget::GetUnitModule() const
{
	if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
	{
		if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
		{
			return DataManager->GetUnitModule();
		}
	}

	return nullptr;
}

UItemModule* UStageListPanelWidget::GetItemModule() const
{
	if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
	{
		if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
		{
			return DataManager->GetItemModule();
		}
	}

	return nullptr;
}

bool UStageListPanelWidget::BuildStageEnemyViewData(
	int32 InChapter,
	int32 InStageNum,
	TArray<FStageEnemyViewData>& OutEnemies,
	int32& OutEnemyLevel) const
{
	OutEnemies.Reset();
	OutEnemyLevel = 1;

	UStageInfoModule* StageInfoModule = GetStageInfoModule();
	UUnitModule* UnitModule = GetUnitModule();
	if (!StageInfoModule || !UnitModule)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageEnemyBuild] Missing module"));
		return false;
	}

	TArray<int32> EnemyUnitIds;
	if (!StageInfoModule->GetEnemyUnitIdsByChapterAndStage(InChapter, InStageNum, EnemyUnitIds))
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageEnemyBuild] Failed to get enemy ids. Chapter=%d Stage=%d"), InChapter, InStageNum);
		return false;
	}

	StageInfoModule->GetEnemyLevelByChapterAndStage(InChapter, InStageNum, OutEnemyLevel);

	for (int32 UnitId : EnemyUnitIds)
	{
		FUnitDataRow UnitRow;
		if (!UnitModule->GetUnitDataRow(UnitId, UnitRow))
		{
			UE_LOG(LogTemp, Warning, TEXT("[StageEnemyBuild] Failed to get unit row. UnitId=%d"), UnitId);
			continue;
		}

		FStageEnemyViewData EnemyData;
		EnemyData.UnitId = UnitId;
		EnemyData.UnitIcon = UnitRow.UnitIcon;
		EnemyData.Grade = UnitRow.Grade;
		EnemyData.UnitType = UnitRow.UnitType;
		EnemyData.TeamType = UnitRow.TeamType;
		EnemyData.Level = OutEnemyLevel;

		OutEnemies.Add(EnemyData);
	}

	return OutEnemies.Num() > 0;
}

bool UStageListPanelWidget::BuildStageRewardData(
	int32 InChapter,
	int32 InStageNum,
	TArray<FRewardViewData>& OutRewards) const
{
	OutRewards.Reset();

	UStageInfoModule* StageInfoModule = GetStageInfoModule();
	UStageRewardModule* StageRewardModule = GetStageRewardModule();
	if (!StageInfoModule || !StageRewardModule)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardBuild] Missing module"));
		return false;
	}

	int32 RewardGroupId = 0;
	if (!StageInfoModule->GetRewardGroupIdByChapterAndStage(InChapter, InStageNum, RewardGroupId))
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardBuild] Failed to get reward group id. Chapter=%d Stage=%d"), InChapter, InStageNum);
		return false;
	}

	TArray<FRewardDataRow> RewardRows;
	const bool bResult = StageRewardModule->GetRewardRowsByStageRewardGroupId(RewardGroupId, RewardRows);
	if (!bResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardBuild] No rewards found. RewardGroupId=%d"), RewardGroupId);
		return false;
	}

	for (const FRewardDataRow& RewardRow : RewardRows)
	{
		FRewardViewData RewardViewData;
		RewardViewData.ItemType = RewardRow.ItemType;
		RewardViewData.DataId = RewardRow.DataId;
		RewardViewData.Amount = RewardRow.Amount;
		RewardViewData.Icon = GetItemModule()->GetTableDataById(RewardViewData.DataId).Icon;
		RewardViewData.DisplayName = FText::FromString(
			GetItemModule()->GetTableDataById(RewardViewData.DataId).Name
		);

		OutRewards.Add(RewardViewData);
	}

	UE_LOG(LogTemp, Log, TEXT("[StageRewardBuild] RewardGroupId=%d RewardCount=%d"), RewardGroupId, OutRewards.Num());
	return OutRewards.Num() > 0;
}

void UStageListPanelWidget::OpenStageDetailPopup(const FStageDetailViewData& InDetailViewData)
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		if (UStageDetailPopupWidget* StageDetailWidget = UIManager->OpenUI<UStageDetailPopupWidget>(EUIType::PopupStageDetail))
		{
			StageDetailWidget->SetStageDetail(InDetailViewData);
		}
	}
}

void UStageListPanelWidget::RefreshChapterHeader()
{
	if (Text_Chapter)
	{
		Text_Chapter->SetText(FText::FromString(FString::Printf(TEXT("Chapter %d"), CurrentChapter)));
	}
}

void UStageListPanelWidget::RefreshChapterButtons()
{
	if (UStageModule* StageModule = GetStageModule())
	{
		if (Button_PrevChapter)
		{
			Button_PrevChapter->SetIsEnabled(StageModule->HasChapter(CurrentChapter - 1));
		}

		if (Button_NextChapter)
		{
			Button_NextChapter->SetIsEnabled(StageModule->HasChapter(CurrentChapter + 1));
		}
	}
	else
	{
		if (Button_PrevChapter)
		{
			Button_PrevChapter->SetIsEnabled(false);
		}

		if (Button_NextChapter)
		{
			Button_NextChapter->SetIsEnabled(false);
		}
	}
}

void UStageListPanelWidget::ClearStageItems()
{
	SpawnedStageItems.Empty();

	if (Grid_StageList)
	{
		Grid_StageList->ClearChildren();
	}
}

void UStageListPanelWidget::RebuildStageItems()
{
	ClearStageItems();

	if (!Grid_StageList || !StageItemWidgetClass)
	{
		return;
	}

	UStageModule* StageModule = GetStageModule();
	if (!StageModule)
	{
		return;
	}

	TArray<FStageEntryData> StageList;
	StageModule->GetStageListViewDataByChapter(CurrentChapter, StageList);

	for (int32 Index = 0; Index < StageList.Num(); ++Index)
	{
		UStageItemWidget* ItemWidget = CreateWidget<UStageItemWidget>(this, StageItemWidgetClass);
		if (!ItemWidget)
		{
			continue;
		}

		ItemWidget->Setup(StageList[Index]);
		ItemWidget->SetSelected(
			StageList[Index].Chapter == SelectedChapter &&
			StageList[Index].StageNum == SelectedStageNum
		);
		ItemWidget->OnStageItemClicked.AddDynamic(this, &UStageListPanelWidget::HandleStageItemClicked);

		const int32 Row = Index / 5;
		const int32 Col = Index % 5;

		Grid_StageList->AddChildToUniformGrid(ItemWidget, Row, Col);
		SpawnedStageItems.Add(ItemWidget);
	}
}

void UStageListPanelWidget::HandlePrevChapter()
{
	if (UStageModule* StageModule = GetStageModule())
	{
		if (StageModule->HasChapter(CurrentChapter - 1))
		{
			CurrentChapter--;
			RefreshUI();
		}
	}
}

void UStageListPanelWidget::HandleNextChapter()
{
	if (UStageModule* StageModule = GetStageModule())
	{
		if (StageModule->HasChapter(CurrentChapter + 1))
		{
			CurrentChapter++;
			RefreshUI();
		}
	}
}

void UStageListPanelWidget::HandleStageItemClicked(int32 InChapter, int32 InStageNum)
{
	SelectedChapter = InChapter;
	SelectedStageNum = InStageNum;

	UE_LOG(LogTemp, Log, TEXT("[StageSelect] Chapter=%d Stage=%d"), SelectedChapter, SelectedStageNum);

	if (UStageModule* StageModule = GetStageModule())
	{
		const int32 Code = StageModule->MakeStageCode(SelectedChapter, SelectedStageNum);
		StageModule->SetSelectedStage(Code);
	}

	FStageDetailViewData DetailViewData;
	DetailViewData.ChapterNum = SelectedChapter;
	DetailViewData.StageNum = SelectedStageNum;

	BuildStageEnemyViewData(
		SelectedChapter,
		SelectedStageNum,
		DetailViewData.Enemies,
		DetailViewData.EnemyLevel
	);

	BuildStageRewardData(
		SelectedChapter,
		SelectedStageNum,
		DetailViewData.Rewards
	);

	OpenStageDetailPopup(DetailViewData);
	RefreshUI();
}
