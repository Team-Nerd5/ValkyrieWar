#include "Widget/Popup/Stage/StageListPanelWidget.h"
#include "Widget/Popup/Stage/StageItemWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/LevelManager.h"

#include "Data/Module/StageModule.h"
#include "Data/Struct/StageEntryData.h"

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

	if (Button_StartStage)
	{
		Button_StartStage->OnClicked.AddDynamic(this, &UStageListPanelWidget::StartStage);

	}

	if (Button_Close)
	{
		Button_Close->OnClicked.AddDynamic(this, &UStageListPanelWidget::CloseStageListPopup);

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

	if (Button_StartStage)
	{
		Button_StartStage->OnClicked.RemoveDynamic(this, &UStageListPanelWidget::StartStage);

	}

	if (Button_Close)
	{
		Button_Close->OnClicked.RemoveDynamic(this, &UStageListPanelWidget::CloseStageListPopup);

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
	RefreshStartButton();
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

void UStageListPanelWidget::RefreshStartButton()
{
	if (!Button_StartStage)
	{
		return;
	}

	const bool bHasSelection = (SelectedChapter > 0 && SelectedStageNum > 0);

	Button_StartStage->SetIsEnabled(bHasSelection);
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
		int32 Code = StageModule->MakeStageCode(SelectedChapter, SelectedStageNum);
		StageModule->SetSelectedStage(Code);
	}

	RefreshUI();
}

void UStageListPanelWidget::StartStage()
{
	if (SelectedChapter <= 0 || SelectedStageNum <= 0) return;

	if (ULevelManager* LevelManager = GetGameInstance()->GetSubsystem<ULevelManager>())
	{
		LevelManager->LoadMap(EMapType::Battle, true);
	}
}

void UStageListPanelWidget::CloseStageListPopup()
{
	CloseUI();
}
